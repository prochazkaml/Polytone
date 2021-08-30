#include "file.h"
#include "player.h"
#include "../diskio.h"
#include "../libs/tinyfiledialogs.h"

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

const char *extensions[2] = { "*.mon", "*.MON" };

void file_new() {
	static dialog_t sure = {
		DIALOG_SIMPLE,
		"Are you sure you want to create a new file?\n \n"
		"Any unsaved changes will be lost!",
		2, { "Go ahead", "Cancel" }
	};

	if(raw_mt != NULL) {
		if(DrawDialog(&sure)) return;
	}

	static dialog_t dialog = {
		DIALOG_NUMBERINPUT,
		"Number of channels: %%%%\n \n"
		"Use the \x7F/\x80 arrow keys to select.\n \n"
		"Ranges from 1 to 12."
		"\0\x03\x01\x0C",	// Parameters: default val 3, min 1, max 12
		2, { "Ok", "Cancel" }
	};

	dialog.buttons = 2;		// DrawDialog() changes it

	if(!DrawDialog(&dialog)) {
		if(raw_mt != NULL) {
			player_stop();
		} else {
			raw_mt = malloc(MAX_MT_SIZE);
		}

		memset(raw_mt, 0, MAX_MT_SIZE);

		strcpy(raw_mt, "\x08MONOTONE");

		raw_mt[0x5B] = 1;	// Version
//		raw_mt[0x5C] = 1;	// Patterns - doesn't really matter for MTPlayer
		raw_mt[0x5D] = dialog.buttons;	// Channels
		raw_mt[0x5E] = 2;	// Cell size

		memset(raw_mt + 0x5F, 0xFF, 0x100);
		raw_mt[0x5F] = 0;

		InitMON("New file");
		lastname = NULL;
	}
}

void file_open() {
	static dialog_t sure = {
		DIALOG_SIMPLE,
		"Are you sure you want to open another file?\n \n"
		"Any unsaved changes will be lost!",
		2, { "Go ahead", "Cancel" }
	};

	if(raw_mt != NULL) {
		if(DrawDialog(&sure)) return;
	}

	char *filename = tinyfd_openFileDialog(
		"Load Monotone module",
		"",
		2,
		extensions,
		"Monotone module",
		0);

	if(filename) {
		player_stop();

		LoadMON(filename);
	} else {
		UpdateStatus("No file selected!");
	}
}

void file_save() {
	if(raw_mt == NULL || lastname == NULL) {
		file_save_as();
	} else {
		player_stop();

		SaveMON(lastname);
	}
}

void file_save_as() {
	if(raw_mt == NULL) {
		UpdateStatus("There is nothing to save.");
	} else {
		char *filename = tinyfd_saveFileDialog(
			"Save Monotone module",
			"",
			2,
			extensions,
			"Monotone module");

		if(filename) {
			player_stop();

			SaveMON(filename);
		} else {
			UpdateStatus("No file selected!");
		}
	}
}

void file_quit() {
	if(raw_mt == NULL) exit(0);

	static dialog_t sure = {
		DIALOG_SIMPLE,
		"Are you sure you want to quit?\n \n"
		"Any unsaved changes will be lost!",
		2, { "Quit Polytone", "Stay" }
	};
	
	if(DrawDialog(&sure) == 0) exit(0);
}
