#include "edit.h"
#include "../tracker.h"
#include "../MTPlayer/mtplayer.h"

menu_t submenu_edit = {
	C(13), C(1), C(19), C(6), 6,
	BLACK, WHITE, {
	
	{ 0, C(0), C(19), C(1), "Cut        [Ctrl+x]" },
	{ 0, C(1), C(19), C(1), "Copy       [Ctrl+c]" },
	{ 0, C(2), C(19), C(1), "Paste      [Ctrl+v]" },
	{ 0, C(3), C(19), C(1), "Select all [Ctrl+a]" },
	{ 0, C(4), C(19), C(1), "Octave up     [F10]" },
	{ 0, C(5), C(19), C(1), "Octave down    [F9]" },
}};

void (*submenu_edit_fn[])() = {
	NULL, NULL, NULL, edit_select_all, edit_octave_up, edit_octave_down
};

void edit_select_all() {
	songstatus_t *status = MTPlayer_GetStatus();
	
	// That's right, we're gonna cheat
	tracker.selected = 0;
	CheckSelection(KMOD_SHIFT);

	tracker._selchannel0 = 0;
	tracker._selchannel1 = status->channels - 1;
	tracker._selrow0 = 0;
	tracker._selrow1 = 0x3F;
	tracker._selcolumn0 = 0;
	tracker._selcolumn1 = 3;

	tracker.update = 1;

	UpdateStatus("Selected entire pattern.");
}

void edit_octave_down() {
	if(tracker.octave > 0) {
		tracker.octave--;
		tracker.update = 1;
	}
}

void edit_octave_up() {
	if(tracker.octave < 9) {
		tracker.octave++;
		tracker.update = 1;
	}
}

