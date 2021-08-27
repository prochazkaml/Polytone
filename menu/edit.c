#include "edit.h"

menu_t submenu_edit = {
	C(13), C(1), C(19), C(6), 6,
	BLACK, WHITE, {
	
	{ 0, C(0), C(19), C(1), "Cut        [Ctrl+x]" },
	{ 0, C(1), C(19), C(1), "Copy       [Ctrl+c]" },
	{ 0, C(2), C(19), C(1), "Paste      [Ctrl+v]" },
	{ 0, C(3), C(19), C(1), "Select all [Ctrl+a]" },
	{ 0, C(4), C(25), C(1), "Octave down    [F9]" },
	{ 0, C(5), C(25), C(1), "Octave up     [F10]" },
}};

void (*submenu_edit_fn[])() = {
	NULL, NULL, NULL, NULL, NULL, NULL
};

