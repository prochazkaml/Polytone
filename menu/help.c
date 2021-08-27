#include "help.h"

menu_t submenu_help = {
	C(24), C(1), C(14), C(2), 2,
	BLACK, WHITE, {
	
	{ 0, C(0), C(14), C(1), "Manual    [F1]" },
	{ 0, C(1), C(14), C(1), "About Polytone" },
}};

void (*submenu_help_fn[])() = {
	NULL, NULL
};
