#include "help.h"

menu_t submenu_help = {
	C(24), C(1), C(14), C(3), 3,
	BLACK, WHITE, {
	
	{ 0, C(0), C(14), C(1), "Manual    [F1]" },
	{ 0, C(1), C(14), C(1), "Effects..." },
	{ 0, C(2), C(14), C(1), "About Polytone" },
}};

void (*submenu_help_fn[])() = {
	NULL, help_effects, help_about
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

void help_effects() {
	static dialog_t dialog = {
		DIALOG_SIMPLE,

		"Effect reference\n \n"
		"0XY = arpeggio (X,Y = 0-15 semitones)         \n"
		"1XX = slide up (XX = 0-255 Hz/tick)           \n"
		"2XX = slide down (XX = 0-255 Hz/tick)         \n"
		"3XX = slide to note (XX = 1-255 Hz/tick)      \n"
		"4XY = vibrato (X = speed 1-15, Y = depth 1-15)\n"
		"BXX = order jump (XX = order number 0-255)    \n"
		"DXX = jump to next pattern (XX = row 0-255)   \n"
		"FXX = set speed (XX = set speed 0-32)         \n"
		"                (XX = set 33-255 tick Hz)     \n \n"

		"If a parameter begins from 1 (3XX, 4XY),\n"
		"then 0 = repeat the last known value.",

		NULL, 1, { "Ok" }
	};

	DrawDialog(&dialog);
}