#include "file.h"

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
	NULL, NULL, NULL, NULL, NULL, NULL, NULL
};
