#include "help.h"

menu_t submenu_help = {
	C(24), C(1), C(14), C(2), 2,
	BLACK, WHITE, {
	
	{ 0, C(0), C(14), C(1), "Manual    [F1]" },
	{ 0, C(1), C(14), C(1), "About Polytone" },
}};

void (*submenu_help_fn[])() = {
	NULL, help_about
};

void help_about() {
	static dialog_t dialog = {
		DIALOG_SIMPLE,

		"Polytone v0.1\n \n"
		"Created by Michal Prochazka, 2021\n \n"
		"Inspired by Trixter / Hornet's Monotone\n \n"
		"https://polytone.prochazka.ml/",

		NULL, 1, { "Ok" }
	};

	DrawDialog(&dialog);
}
