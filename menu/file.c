#include "file.h"
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
	NULL, file_open, NULL, NULL, NULL, NULL, file_quit
};

const char *extensions[2] = { "*.mon", "*.MON" };

void file_open() {
	char *filename = tinyfd_openFileDialog(
		"Load Monotone module",
		"",
		2,
		extensions,
		"Monotone module",
		0);

	if(filename) {
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
