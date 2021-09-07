#include "edit.h"
#include "player.h"
#include "../diskio.h"
#include "../tracker.h"

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
	edit_cut, edit_copy, edit_paste,
	edit_select_all, edit_octave_up, edit_octave_down
};

void edit_cut() {
	player_stop();

	if(buffer == NULL) {
		UpdateStatus("There is nowhere to cut from.");
		return;
	}

	edit_copy();
	UpdateStatus("Cut %d rows.", DeleteSelectedBox());

	tracker.update = 1;
}

#define append(s, ...) sprintf(s + strlen(s), __VA_ARGS__)

void edit_copy() {
	player_stop();

	if(buffer == NULL) {
		UpdateStatus("There is nowhere to copy from.");
		return;
	}

	if(!tracker.selected) {
		CheckSelection(KMOD_SHIFT);
		UpdateSelectedBox();
	}

	char str[16384] = "POLYTONE_CLIPBOARD\n";	// 16k should be enough, right?

	append(str, "%d %d %d %d",
		tracker._selrow1 - tracker._selrow0 + 1,
		tracker._selchannel1 - tracker._selchannel0 + 1,
		tracker._selcolumn0, tracker._selcolumn1);

	int oldch = tracker.channel, oldrow = tracker.row;

	for(int r = tracker._selrow0; r <= tracker._selrow1; r++) {
		append(str, "\n");
		tracker.row = r;

		for(int i = tracker._selchannel0 * 4 + tracker._selcolumn0;
			i <= tracker._selchannel1 * 4 + tracker._selcolumn1; i++) {

			tracker.channel = i / 4;

			append(str, "%d ", GetData(i % 4));
		}
	}

	SDL_SetClipboardText(str);

	tracker.selected = 0;
	tracker.channel = oldch;
	tracker.row = oldrow;
	tracker.update = 1;

	UpdateStatus("Copied %d rows.", tracker._selrow1 - tracker._selrow0 + 1);
}

void edit_paste() {
	player_stop();

	if(buffer == NULL) {
		UpdateStatus("There is nowhere to paste to.");
		return;
	}

	tracker.selected = 0;

	char *str = SDL_GetClipboardText();
	char *data = str + 19;

	if(str == NULL) {
		UpdateStatus("Error pasting: %s", SDL_GetError());
		return;
	}

	int rows = strtol(data, &data, 10),
		channels = strtol(data, &data, 10),
		col0 = strtol(data, &data, 10),
		col1 = strtol(data, &data, 10);

	if(memcmp(str, "POLYTONE_CLIPBOARD\n", 19)) {
		UpdateStatus("The clipboard contains invalid data.");
		return;
	}
	
	int oldch = tracker.channel, oldrow = tracker.row;

	if(oldrow + rows > 0x40) rows = 0x40 - oldrow; 
	if(oldch + channels > buffer->channels)
		channels = buffer->channels - oldch;

	for(int r = 0; r < rows; r++) {
		for(int i = col0; i <= (channels - 1) * 4 + col1; i++) {
			tracker.row = r + oldrow;
			tracker.channel = i / 4 + oldch;

			PutData(i % 4, strtol(data, &data, 10));
		}
	}

	UpdateStatus("Pasted %d rows.", rows);

	tracker.channel = oldch;
	tracker.row = (rows + oldrow) & 0x3F;

	tracker.update = 1;

	SDL_free(str);
}

void edit_select_all() {
	player_stop();

	if(buffer == NULL) {
		UpdateStatus("There is nothing to select.");
		return;
	}

	// That's right, we're gonna cheat
	tracker.selected = 0;
	CheckSelection(KMOD_SHIFT);

	tracker._selchannel0 = 0;
	tracker._selchannel1 = buffer->channels - 1;
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

