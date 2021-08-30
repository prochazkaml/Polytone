#include "file.h"
#include "player.h"
#include "../diskio.h"
#include "../libs/tinyfiledialogs.h"

menu_t submenu_file = {
	C(1), C(1), C(16), C(7), 7,
	BLACK, WHITE, {
	
	{ 0, C(0), C(16), C(1), "New     [Ctrl+n]" },
	{ 0, C(1), C(16), C(1), "Open    [Ctrl+o]" },
	{ 0, C(2), C(16), C(1), "Save    [Ctrl+s]" },
	{ 0, C(3), C(16), C(1), "Save as [Ctrl+S]" },
	{ 0, C(4), C(16), C(1), "Export  [Ctrl+e]" },
	{ 0, C(5), C(16), C(1), "Setup   [Ctrl+,]" },
	{ 0, C(6), C(16), C(1), "Quit    [Ctrl+q]" },
}};

void (*submenu_file_fn[])() = {
	file_new, file_open, NULL, NULL, NULL, NULL, file_quit
};

const char *extensions[2] = { "*.mon", "*.MON" };

void file_new() {
	if(raw_mt != NULL) {
		player_stop();
	} else {
		raw_mt = malloc(MAX_MT_SIZE);
	}

	memset(raw_mt, 0, MAX_MT_SIZE);

	strcpy(raw_mt, "\x08MONOTONE");

	raw_mt[0x5B] = 1;	// Version
//	raw_mt[0x5C] = 1;	// Patterns - doesn't really matter for MTPlayer
	raw_mt[0x5D] = 3;	// Channels
	raw_mt[0x5E] = 2;	// Cell size
	raw_mt[0x5F] = 0;
	raw_mt[0x60] = 0xFF;

	InitMON("New file");
	lastname = NULL;
}

void file_open() {
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

void file_quit() {
	if(raw_mt == NULL) exit(0);

	static dialog_t sure = {
		DIALOG_SIMPLE,
		"Are you sure you want to quit?\n \n"
		"Any unsaved data will be lost!",
		2, { "Quit Polytone", "Stay" }
	};
	
	if(DrawDialog(&sure) == 0) exit(0);
}
