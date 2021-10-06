#include "tracker.h"
#include "sdl.h"
#include "diskio.h"
#include <stdio.h>

tracker_t tracker;

const uint8_t notekeys[] = {
	SDL_SCANCODE_Z, SDL_SCANCODE_S,
	SDL_SCANCODE_X, SDL_SCANCODE_D,
	SDL_SCANCODE_C,
	SDL_SCANCODE_V, SDL_SCANCODE_G,
	SDL_SCANCODE_B, SDL_SCANCODE_H,
	SDL_SCANCODE_N, SDL_SCANCODE_J,
	SDL_SCANCODE_M,

	SDL_SCANCODE_Q, SDL_SCANCODE_2,
	SDL_SCANCODE_W, SDL_SCANCODE_3,
	SDL_SCANCODE_E,
	SDL_SCANCODE_R, SDL_SCANCODE_5,
	SDL_SCANCODE_T, SDL_SCANCODE_6,
	SDL_SCANCODE_Y, SDL_SCANCODE_7,
	SDL_SCANCODE_U,

	SDL_SCANCODE_I, SDL_SCANCODE_9,
	SDL_SCANCODE_O, SDL_SCANCODE_0,
	SDL_SCANCODE_P,
	SDL_SCANCODE_LEFTBRACKET, SDL_SCANCODE_EQUALS,
	SDL_SCANCODE_RIGHTBRACKET
};

const uint8_t effvalkeys[] = {
	SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
	SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6,	SDL_SCANCODE_7,
	SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
	SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F
};

const uint8_t ignorekeys[] = {
	SDL_SCANCODE_LCTRL, SDL_SCANCODE_LSHIFT, 
	SDL_SCANCODE_RCTRL, SDL_SCANCODE_LGUI,
	SDL_SCANCODE_LALT, SDL_SCANCODE_LGUI,
	SDL_SCANCODE_RALT, SDL_SCANCODE_RGUI
};

const int offsetstart[] = { 4, 36, 44, 52 };
const int offsetend[] = { 28, 44, 52, 60 };

void MoveCursor(int amount) {
	tracker.row += amount;

	if(amount > 0) {
		if(tracker.selected) {
			if(tracker.row >= 0x40) tracker.row -= 0x40;
		} else while(tracker.row >= 0x40) {
			tracker.row -= 0x40;
			tracker.order++;

			if(tracker.order >= buffer->orders) tracker.order = 0; 
		}
	} else if(amount < 0) {
		if(tracker.selected) {
			if(tracker.row < 0) tracker.row += 0x40;
		} else while(tracker.row < 0) {
			tracker.row += 0x40;
			tracker.order--;

			if(tracker.order < 0) tracker.order = buffer->orders - 1;
		}
	}
}

#define currpat buffer->data[buffer->ordertable[tracker.order]]
#define currrow currpat[tracker.row]

void PutData(int col, int val) {
	unpacked_t *data = &currrow[tracker.channel];

	switch(col) {
		case 0:
			data->note = val;
			break;

		case 1:
			data->effect = val & 0xF;

			if(data->effect == 0 && data->effectval == 0) {
				PutData(2, 4);
				PutData(3, 7);
			}

			break;

		case 2:
			data->effectval &= 0x0F;
			data->effectval |= val << 4;
			break;

		case 3:
			data->effectval &= 0xF0;
			data->effectval |= val & 0xF;
			break;
	}
}

int GetData(int col) {
	unpacked_t *data = &currrow[tracker.channel];

	switch(col) {
		case 0:
			return data->note;

		case 1:
			return data->effect;

		case 2:
			return data->effectval >> 4;

		case 3:
			return data->effectval & 0x0F;
	}

	return 0;
}

void StopSelection() {
	if(tracker.selected) {
		tracker.selected = 0;
		UpdateStatus("The current selection was abandoned.");
	}
}

void CheckSelection(int mod) {
	if((mod & KMOD_SHIFT) && !tracker.selected) {
		tracker.selected = 1;
		tracker.selrow = tracker.row;
		tracker.selchannel = tracker.channel;
		tracker.selcolumn = tracker.column;
		UpdateStatus("Selection mode active!");
	} else if(!(mod & KMOD_SHIFT)) {
		StopSelection();
	}
}

void UpdateSelectedBox() {
	if(tracker.selrow < tracker.row) {
		tracker._selrow0 = tracker.selrow;
		tracker._selrow1 = tracker.row;
	} else {
		tracker._selrow1 = tracker.selrow;
		tracker._selrow0 = tracker.row;
	}

	if((tracker.selchannel * 4 + tracker.selcolumn) < 
		(tracker.channel * 4 + tracker.column)) {
		
		tracker._selchannel0 = tracker.selchannel;
		tracker._selchannel1 = tracker.channel;
		tracker._selcolumn0 = tracker.selcolumn;
		tracker._selcolumn1 = tracker.column;
	} else {
		tracker._selchannel0 = tracker.channel;
		tracker._selchannel1 = tracker.selchannel;
		tracker._selcolumn0 = tracker.column;
		tracker._selcolumn1 = tracker.selcolumn;
	}
}

int IsRowInSelected(int row) {
	return row >= tracker._selrow0 && row <= tracker._selrow1;
}

int GetSelectedLeftX() {
	return 32 + tracker._selchannel0 * 64 + offsetstart[tracker._selcolumn0];
}

int GetSelectedRightX() {
	return 32 + tracker._selchannel1 * 64 + offsetend[tracker._selcolumn1];
}

int DeleteSelectedBox() {
	for(int r = tracker._selrow0; r <= tracker._selrow1; r++) {
		for(int i = tracker._selchannel0 * 4 + tracker._selcolumn0;
			i <= tracker._selchannel1 * 4 + tracker._selcolumn1; i++) {

			tracker.row = r;
			tracker.channel = i / 4;
			tracker.column = i % 4;

			switch(tracker.column) {
				case 0:
					InsertNote(0);
					break;

				case 1:
					InsertEffect(0x10);	// same as 0, but doesn't trigger autofill
					break;
				
				case 2:
				case 3:
					InsertEffectValue(tracker.column - 2, 0);
					break;
			}
		}
	}

	tracker.selected = 0;
	tracker.row = tracker._selrow0;
	tracker.channel = tracker._selchannel0;
	tracker.column = tracker._selcolumn0;

	UpdateStatus("The current selection's data was cleared.");

	return tracker._selrow1 - tracker._selrow0 + 1;
}

void MoveDataUp() {
	for(int i = tracker.row; i < 0x3F; i++) {
		currpat[i][tracker.channel] =
			currpat[i + 1][tracker.channel];
	}

	for(int c = 0; c < buffer->channels; c++) {
		currpat[0x3F][tracker.channel].note = 0;
		currpat[0x3F][tracker.channel].effect = 0;
		currpat[0x3F][tracker.channel].effectval = 0;
	}
}

void ParseKey(int mod, int scancode) {
	for(int i = 0; i < sizeof(ignorekeys); i++) {
		if(scancode == ignorekeys[i]) return;
	}

	printf("%d %d\n", mod, scancode);

	if(!playing) {
		// Parse editor keys
		
		switch(scancode) {
			case SDL_SCANCODE_UP:
				CheckSelection(mod);
				MoveCursor((mod & KMOD_CTRL) ? (-16) : (-1));
				break;
				
			case SDL_SCANCODE_DOWN:
				CheckSelection(mod);
				MoveCursor((mod & KMOD_CTRL) ? 16 : 1);
				break;

			case SDL_SCANCODE_LEFT:
				if(mod & KMOD_CTRL) {
					StopSelection();

					if(buffer->ordertable[tracker.order] > 0)
						buffer->ordertable[tracker.order]--;

					PTPlayer_Reset(buffer);
				} else {
					CheckSelection(mod);

					tracker.column--;
					if(tracker.column < 0) {
						tracker.column = 3;
						tracker.channel--;
						if(tracker.channel < 0) tracker.channel = buffer->channels - 1;
					}
				}

				break;

			case SDL_SCANCODE_RIGHT:
				if(mod & KMOD_CTRL) {
					StopSelection();

					if(buffer->ordertable[tracker.order] < 0xFF)
						buffer->ordertable[tracker.order]++;

					PTPlayer_Reset(buffer);
				} else {
					CheckSelection(mod);

					tracker.column++;
					if(tracker.column >= 4) {
						tracker.column = 0;
						tracker.channel++;
						if(tracker.channel >= buffer->channels) tracker.channel = 0;
					}
				}

				break;

			case SDL_SCANCODE_PAGEUP:
				CheckSelection(mod);
				MoveCursor(-64);
				break;
				
			case SDL_SCANCODE_PAGEDOWN:
				CheckSelection(mod);
				MoveCursor(64);
				break;
				
			case SDL_SCANCODE_HOME:
				CheckSelection(mod);
				tracker.row = 0;
				break;

			case SDL_SCANCODE_END:
				CheckSelection(mod);
				tracker.row = 0x3F;
				break;

			case SDL_SCANCODE_SPACE:
				if(!(mod & KMOD_CTRL)) {
					StopSelection();
					InsertNote(0x7F);
					MoveCursor(1);
					break;
				}

			case SDL_SCANCODE_INSERT:
				StopSelection();

				for(int i = 0x3F; i > tracker.row; i--) {
					currpat[i][tracker.channel] = currpat[i - 1][tracker.channel];
				}

				currrow[tracker.channel].note = 0;
				currrow[tracker.channel].effect = 0;
				currrow[tracker.channel].effectval = 0;
				break;

			case SDL_SCANCODE_BACKSPACE:
				if(tracker.selected) {
					DeleteSelectedBox();
				} else {
					if(tracker.column) {
						InsertEffect(0);
						InsertEffectValue(0, 0);
						InsertEffectValue(1, 0);
					} else {
						InsertNote(0);
					}

					MoveCursor(1);
				}

				break;

			case SDL_SCANCODE_DELETE:
				if(tracker.selected) {
					for(int r = DeleteSelectedBox(); r > 0; r--) {
						for(int i = tracker._selchannel0; i <= tracker._selchannel1; i++) {
							tracker.channel = i;
							MoveDataUp();
						}
					}

					tracker.channel = tracker._selchannel0;
				} else {
					MoveDataUp();
				}

				break;
		}

		// Parse note/effect keys

		switch(tracker.column) {
			case 0:
				for(int i = 0; i < sizeof(notekeys); i++) {
					if(scancode == notekeys[i]) {
						StopSelection();
						int note = i + tracker.octave * 12 - 8;
						if(note > 0 && note <= 88) {
							immnotedelay = 3;
							immnote = note;

							InsertNote(note);
							MoveCursor(1);
						}

						break;
					}
				}

				break;

			case 1:
				for(int i = 0; i < sizeof(effvalkeys); i++) {
					if(scancode == effvalkeys[i]) {
						StopSelection();
						InsertEffect(i);
						tracker.column++;
					}
				}

				break;

			case 2:
			case 3:
				for(int i = 0; i < sizeof(effvalkeys); i++) {
					if(scancode == effvalkeys[i]) {
						StopSelection();
						InsertEffectValue(tracker.column - 2, i);
						
						if(tracker.column == 2) {
							tracker.column++;
						} else {
							tracker.column = 1;
							MoveCursor(1);
						}
					}
				}

				break;
		}

		tracker.update = 1;
	} else {
		// Parse player keys

		if((scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_0) ||
			scancode == SDL_SCANCODE_MINUS || scancode == SDL_SCANCODE_EQUALS) {

			if(scancode > SDL_SCANCODE_0)
				scancode -= (SDL_SCANCODE_MINUS - 10);
			else
				scancode -= SDL_SCANCODE_1;

			tracker.s->channel[scancode].enabled ^= 1;
		}
	}

	if(tracker.selected) UpdateSelectedBox();
}

const char *notes[12] = {
	"C-", "C#", "D-", "D#", "E-", "F-",
	"F#", "G-", "G#", "A-", "A#", "B-"
};

const uint32_t effectcolors[16] = {
	0xFFFFB0B0, 0xFFB0FFFF, 0xFFB0FFFF, 0xFFB0FFFF, 
	0xFFB0FFFF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 
	0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFB0B0FF,
	0xFF0000FF, 0xFFB0B0FF, 0xFF0000FF, 0xFFFFB0B0
};

void DrawRow(int y, int order, int row) {
	Printf(8, y, WHITE, "%02X", row);

	for(int c = 0; c < buffer->channels; c++) {
		int n = buffer->data[buffer->ordertable[order]][row][c].note;
		int e = buffer->data[buffer->ordertable[order]][row][c].effect;
		int v = buffer->data[buffer->ordertable[order]][row][c].effectval;

		if(n == 127) {
			Printf(36 + c * 64, y, 0xFF8080FF, "OFF");
		} else if(n) {
			if(e != 3)
				Printf(36 + c * 64, y, WHITE, "%s%d",
					notes[(n + 8) % 12], (n + 8) / 12);
			else
				Printf(36 + c * 64, y, effectcolors[3], "%s%d",
					notes[(n + 8) % 12], (n + 8) / 12);
		} else {
			Printf(36 + c * 64, y, 0xFF808080, "---");
		}

		uint32_t color = effectcolors[e];

		if(e || v) {
			Printf(68 + c * 64, y, color, "%c", "0123456789ABCDEF"[e]);

			if(e == 0 || e == 4) {
				Printf(76 + c * 64, y, 0xFFC0C0FF, "%X", v >> 4);
				Printf(84 + c * 64, y, 0xFFC0FFC0, "%X", v & 15);
			} else {
				Printf(76 + c * 64, y, color, "%02X", v);
			}
		} else {
			Printf(68 + c * 64, y, 0xFF808080, "---");
		}
	}
}

#define TRACKERWIN_Y (48 + (S_HEIGHT % 8))
#define CHINFO_Y (13 + (S_HEIGHT % 8) / 2)
#define BARWIDTH 2
#define ORDERWIDTH 48
#define TRACKERWIN_W (S_WIDTH - ORDERWIDTH)
#define MIDDLEROW_Y (S_HEIGHT - 8 - TRACKERWIN_Y) / 16 * 8 + TRACKERWIN_Y

void RenderTracker() {
	SDL_Rect bar = { .x = 0, .y = TRACKERWIN_Y - BARWIDTH - 1, .w = S_WIDTH, .h = BARWIDTH};
	SDL_Rect rect = { .x = 0, .y = 8, .w = TRACKERWIN_W, .h = S_HEIGHT - 16 };
	SDL_Rect order = { .x = TRACKERWIN_W, .y = 8, .w = ORDERWIDTH, .h = S_HEIGHT - 16 };
	SDL_Rect orderbar = { .x = TRACKERWIN_W, .y = 8, .w = BARWIDTH, .h = S_HEIGHT - 16 };

	if(tracker.update) {
		SDL_FillRect(surface, &rect, 0xFF222222);

		rect.x = 32;
		rect.w = 64;

		while(rect.x < TRACKERWIN_W) {
			SDL_FillRect(surface, &rect, 0xFF333333);

			rect.x += 128;
		}

		for(int i = 0; i < S_WIDTH * 8; i++) {
			PIXEL(i, MIDDLEROW_Y) += 0x303030;
		}

		Printf(8, CHINFO_Y + 10, WHITE, "%02X", buffer->ordertable[tracker.order]);

		if(playing) {
			if(!stopdelay) {
				UpdateStatus("Playing %d.%02ds (speed %d @ %d Hz)...",
					samples / 100, samples % 100, tracker.s->tempo, tracker.s->audiospeed);
			}
			
			tracker.selected = 0;

			for(int c = 0; c < buffer->channels; c++) {
				int color = tracker.s->channel[c].enabled ? WHITE : 0xFF808080;
				Printf(36 + c * 64, CHINFO_Y, color, "Ch %d", c + 1);
				Printf(36 + c * 64, CHINFO_Y + 10, color, tracker.s->channel[c].active ? "%d Hz" : "-", tracker.s->channel[c].freq);

				int ctr = tracker.s->channel[c].ctr;

				if(ctr != tracker.old_ctr[c] && tracker.ch_ctr[c] < 14) tracker.ch_ctr[c] = 14;

				if(tracker.ch_ctr[c] < 0) tracker.ch_ctr[c] = 0;

				if(ctr < tracker.old_ctr[c]) tracker.ch_ctr[c] = 27;

				for(int x = 0; x < tracker.ch_ctr[c]; x++) {
					for(int y = 0; y < 7; y++) {
						PIXEL(37 + c * 64 + x * 2, CHINFO_Y + 20 + y) = 
							(x < 14) ? 0xFF00FF00 : ((x < 21) ? 0xFF00FFFF : 0xFF0000FF);
					}
				}

				tracker.ch_ctr[c]--;
				tracker.old_ctr[c] = ctr;
			}
		} else {
			for(int c = 0; c < buffer->channels; c++) {
				Printf(36 + c * 64, CHINFO_Y, WHITE, "Ch %d", c + 1);
				Printf(36 + c * 64, CHINFO_Y + 10, 0xFF808080, "-", c + 1);
			}

			if(!tracker.selected) {
				for(int y = MIDDLEROW_Y; y < MIDDLEROW_Y + 8; y++) {
					for(int x = 0; x < 2; x++) {
						PIXEL(x + 32 + tracker.channel * 64, y) = WHITE;
						PIXEL(x + 94 + tracker.channel * 64, y) = WHITE;
					}

					for(int x = offsetstart[tracker.column]; x < offsetend[tracker.column]; x++) {
						PIXEL(x + 32 + tracker.channel * 64, y) += 0x202020;
					}
				}
			}
		}

		int row = tracker.row - (S_HEIGHT - 8 - TRACKERWIN_Y) / 16;
		int drawn = 0;

		for(int y = TRACKERWIN_Y; y < S_HEIGHT - 8; y += 8) {
			if(row >= 0 && row < 64) {
				drawn = 1;

				if(tracker.selected) {
					if(IsRowInSelected(row)) for(int i = y; i < y + 8; i++) {
						for(int x = GetSelectedLeftX(); x < GetSelectedRightX(); x++) {
							PIXEL(x, i) += 0x404040;
						}
					}
				}

				DrawRow(y, tracker.order, row);
			} else {
				if(!drawn) {
					if(tracker.order > 0)
						DrawRow(y, tracker.order - 1, row + 64);
				} else {
					if(tracker.order < buffer->orders - 1)
						DrawRow(y, tracker.order + 1, row - 64);
				}

				for(int i = 0; i < S_WIDTH * 8; i++) {
					PIXEL(i, y) &= 0x7FFFFFFF;
				}
			}

			row++;
		}

		SDL_FillRect(surface, &order, 0xFF222222);
		SDL_FillRect(surface, &orderbar, WHITE);

		Printf(TRACKERWIN_W + 5, CHINFO_Y, WHITE, "Order");
		Printf(TRACKERWIN_W + 5, CHINFO_Y + 10, WHITE, "%02X/%02X", tracker.order, buffer->orders);
		Printf(TRACKERWIN_W + 5, CHINFO_Y + 20, WHITE, "Oct:%d", tracker.octave);

		int order = tracker.order - (S_HEIGHT - 8 - TRACKERWIN_Y) / 16;

		for(int y = TRACKERWIN_Y; y < S_HEIGHT - 8; y += 8) {
			if(order >= 0 && order < buffer->orders) {
				Printf(TRACKERWIN_W + 5, y, (order == tracker.order) ? WHITE : 0xFF808080,
					"%02X:%02X", order, buffer->ordertable[order]);
			}

			order++;
		}

		SDL_FillRect(surface, &bar, 0xFFFFFFFF);

		tracker.update = 0;
	}
}
