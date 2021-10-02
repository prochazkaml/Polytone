#include "file.h"
#include "player.h"
#include "../tracker.h"
#include "../diskio.h"
#include "../libs/tinyfiledialogs.h"
#include <string.h>
#include <libgen.h>

menu_t submenu_file = {
	C(1), C(1), C(16), C(6), 6,
	BLACK, WHITE, {
	
	{ 0, C(0), C(16), C(1), "New     [Ctrl+n]" },
	{ 0, C(1), C(16), C(1), "Open    [Ctrl+o]" },
	{ 0, C(2), C(16), C(1), "Save    [Ctrl+s]" },
	{ 0, C(3), C(16), C(1), "Save as [Ctrl+S]" },
	{ 0, C(4), C(16), C(1), "Export  [Ctrl+e]" },
	{ 0, C(5), C(16), C(1), "Quit    [Ctrl+q]" },
}};

void (*submenu_file_fn[])() = {
	file_new, file_open, file_save, file_save_as, file_export, file_quit
};

const char *loadextensions[4] = { "*.mon", "*.MON", "*.pol", "*.POL" };
const char *saveextensions[2] = { "*.pol", "*.POL" };

void file_new() {
	static dialog_t sure = {
		DIALOG_SIMPLE,
		"Are you sure you want to create a new file?\n \n"
		"Any unsaved changes will be lost!",
		NULL, 2, { "Go ahead", "Cancel" }
	};

	if(buffer != NULL) {
		if(DrawDialog(&sure)) return;
	}

	dialog_numberparam_t params = { 3, 1, 12 };

	dialog_t dialog = {
		DIALOG_NUMBERINPUT,
		"Number of channels: %%%%\n \n"
		"Use the \x7F/\x80 arrow keys to select.\n \n"
		"Ranges from 1 to 12.",
		&params, 2, { "Ok", "Cancel" }
	};

	if(!DrawDialog(&dialog)) {
		if(buffer != NULL) {
			player_stop();
		}

		uint8_t basicfile[15];

		memcpy(basicfile, "\x08POLYTONE\x01\x01\x03\x01\x00\xC0", 15);
		
		basicfile[0x0B] = params.def;

		InitPOL(lastname = NULL, basicfile);
	}
}

void file_open() {
	static dialog_t sure = {
		DIALOG_SIMPLE,
		"Are you sure you want to open another file?\n \n"
		"Any unsaved changes will be lost!",
		NULL, 2, { "Go ahead", "Cancel" }
	};

	if(buffer != NULL) {
		if(DrawDialog(&sure)) return;
	}

	char *filename = tinyfd_openFileDialog(
		"Load Monotone module",
		"",
		4,
		loadextensions,
		NULL,
		0);

	if(filename) {
		player_stop();

		LoadPOL(filename);
	} else {
		UpdateStatus("No file selected!");
	}
}

void file_save() {
	if(buffer == NULL || lastname == NULL ||
		strcasecmp(lastname + strlen(lastname) - 4, ".pol")) {
		
		file_save_as();
	} else {
		player_stop();

		SavePOL(lastname);
	}
}

void file_save_as() {
	if(buffer == NULL) {
		UpdateStatus("There is nothing to save.");
	} else {
		char *filename = tinyfd_saveFileDialog(
			"Save Monotone module",
			"",
			2,
			saveextensions,
			NULL);

		if(filename) {
			player_stop();

			SavePOL(filename);
		} else {
			UpdateStatus("No file selected!");
		}
	}
}

void _export_csv(FILE *file) {
	int old;

	while(1) {
		old = tracker.s->order * 64 + tracker.s->row;

		PTPlayer_ProcessTick();

		if((tracker.s->order * 64 + tracker.s->row) < old) break;

		fprintf(file, "%d,%d,%d", tracker.s->order, tracker.s->row, tracker.s->tempo - tracker.s->tempotick);

		for(int c = 0; c < tracker.s->buf->channels; c++) {
			int f = tracker.s->channel[c].freq;
			fprintf(file, ",%d", tracker.s->channel[c].active ? f : 0);
		}

		fprintf(file, "\n");
	}

}

void _export_wav(FILE *file) {
	int old;
	int16_t buf;

	int samples = 0;

	// Write the data first

	fseek(file, 44, SEEK_SET);

	while(1) {
		old = tracker.s->order * 64 + tracker.s->row;

		PTPlayer_PlayInt16(&buf, 1, 44100);

		if((tracker.s->order * 64 + tracker.s->row) < old) break;

		fputc(buf & 0xFF, file);
		fputc(buf >> 8, file);

		samples++;
	}

	// Generate a valid header

	fseek(file, 0, SEEK_SET);

	fputs("RIFF", file);
	fput32(36 + samples * 2, file);
	fputs("WAVEfmt ", file);
	fput32(16, file);
	fput16(1, file);
	fput16(1, file);
	fput32(44100, file);
	fput32(44100 * 2, file);
	fput16(2, file);
	fput16(16, file);
	fputs("data", file);
	fput32(samples * 2, file);
}

const char *exportextensions[][2] = {
	{ "*.csv", "*.CSV" },
	{ "*.wav", "*.WAV" }
};

void (*_export_fn[])(FILE *) = {
	_export_csv, _export_wav
};

void file_export() {
	if(buffer == NULL) {
		UpdateStatus("There is nothing to export.");
	} else {
		static dialog_t format = {
			DIALOG_SIMPLE,
			"What format do you want to export as?",
			NULL, 3, { "CSV", "WAV", "Cancel" }
		};

		int response = DrawDialog(&format);

		if(response < (format.buttons - 1)) {
			char *filename = tinyfd_saveFileDialog(
				"Export file",
				"",
				2,
				exportextensions[response],
				NULL);

			if(filename) {
				player_stop();

				FILE *f = diskio_fopen(filename, "wb");

				if(f == NULL) {
					UpdateStatus("Could open export %s!", basename(filename));
				} else {
					_export_fn[response](f);
					fclose(f);

					UpdateStatus("Exported %s!", basename(filename));
				}
			} else {
				UpdateStatus("No file selected!");
			}
		}
	}
}

void file_quit() {
	if(buffer == NULL) exit(0);

	static dialog_t sure = {
		DIALOG_SIMPLE,
		"Are you sure you want to quit?\n \n"
		"Any unsaved changes will be lost!",
		NULL, 2, { "Quit Polytone", "Stay" }
	};
	
	if(DrawDialog(&sure) == 0) exit(0);
}
