#include "file.h"
#include "player.h"
#include "../diskio.h"
#include "../libs/tinyfiledialogs.h"
#include <string.h>

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
	file_new, file_open, file_save, file_save_as, NULL, file_quit
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
