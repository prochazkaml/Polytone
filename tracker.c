#include "tracker.h"
#include "sdl.h"
#include "MTPlayer/mtplayer.h"

tracker_t tracker;

void ParseKey(int mod, int scancode) {
	songstatus_t *status = MTPlayer_GetStatus();

	if(SDL_GetAudioStatus() != SDL_AUDIO_PLAYING) {
		// Parse editor keys
		
		switch(scancode) {
			case SDL_SCANCODE_UP:
				if(tracker.row == 0 && tracker.order > 0) {
					tracker.order--;
					tracker.row = 0x3F;
				} else if(tracker.row > 0) {
					tracker.row--;
				} else {
					tracker.order = status->orders - 1;
					tracker.row = 0x3F;
				}
				
				break;
				
			case SDL_SCANCODE_DOWN:
				if(tracker.row == 0x3F && tracker.order < status->orders - 1) {
					tracker.order++;
					tracker.row = 0;
				} else if(tracker.row < 0x3F) {
					tracker.row++;
				} else {
					tracker.order = 0;
					tracker.row = 0;
				}
				
				break;

			case SDL_SCANCODE_PAGEUP:
				if(tracker.order > 0) {
					tracker.order--;
				} else {
					tracker.order = status->orders - 1;
				}

				break;
				
			case SDL_SCANCODE_PAGEDOWN:
				if(tracker.order < status->orders - 1) {
					tracker.order++;
				} else {
					tracker.order = 0;
				}

				break;
				
			case SDL_SCANCODE_HOME:
				tracker.row = 0;
				break;

			case SDL_SCANCODE_END:
				tracker.row = 0x3F;
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

			status->channel[scancode].enabled ^= 1;
		}
	}
}

const char *notes[12] = {
	"C-", "C#", "D-", "D#", "E-", "F-",
	"F#", "G-", "G#", "A-", "A#", "B-"
};

const char effects[8] = "01234BDF";
const uint32_t effectcolors[8] = {
	0xFFFFB0B0, 0xFFB0FFFF, 0xFFB0FFFF, 0xFFB0FFFF, 
	0xFFB0FFFF, 0xFFB0B0FF, 0xFFB0B0FF, 0xFFFFB0B0
};

void DrawRow(int y, int order, int row, songstatus_t *status) {
	Printf(8, y, WHITE, "%02X", row);

	for(int c = 0; c < status->channels; c++) {
		uint16_t data = status->data[(status->ordertable[order] * 64 + row) * status->channels + c];

		int n = data >> 9;
		int e = (data >> 6) & 7;
		int v = data & 63;

		if(n == 127) {
			Printf(36 + c * 64, y, 0xFF7777BB, "OFF");
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
			Printf(68 + c * 64, y, color, "%c", effects[e]);

			if(e == 0 || e == 4) {
				Printf(76 + c * 64, y, 0xFFC0C0FF, "%X", v >> 3);
				Printf(84 + c * 64, y, 0xFFC0FFC0, "%X", v & 7);
			} else {
				Printf(76 + c * 64, y, color, "%02X", v);
			}
		} else {
			Printf(68 + c * 64, y, 0xFF808080, "---");
		}
	}
}

#define TRACKERWIN_Y 48
#define BARWIDTH 2
#define ORDERWIDTH 48
#define TRACKERWIN_W (S_WIDTH - ORDERWIDTH)
#define MIDDLEROW_Y (S_HEIGHT - 8 - TRACKERWIN_Y) / 16 * 8 + TRACKERWIN_Y

void RenderTracker() {
	songstatus_t *status = MTPlayer_GetStatus();
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

		Printf(8, 23, WHITE, "%02X", status->ordertable[tracker.order]);

		if(SDL_GetAudioStatus() == SDL_AUDIO_PLAYING) {
			UpdateStatus("Playing %d.%02ds @ speed %d...",
				samples / 100, samples % 100, status->tempo);

			for(int c = 0; c < status->channels; c++) {
				int color = status->channel[c].enabled ? WHITE : 0xFF808080;
				Printf(36 + c * 64, 13, color, "Ch %d", c + 1);
				Printf(36 + c * 64, 23, color, status->channel[c].active ? "%d Hz" : "-", status->channel[c].freq);

				int ctr = status->channel[c].ctr;

				if(ctr != tracker.old_ctr[c] && tracker.ch_ctr[c] < 14) tracker.ch_ctr[c] = 14;

				if(tracker.ch_ctr[c] < 0) tracker.ch_ctr[c] = 0;

				if(ctr < tracker.old_ctr[c]) tracker.ch_ctr[c] = 27;

				for(int x = 0; x < tracker.ch_ctr[c]; x++) {
					for(int y = 0; y < 7; y++) {
						PIXEL(37 + c * 64 + x * 2, 33 + y) = 
							(x < 14) ? 0xFF00FF00 : ((x < 21) ? 0xFF00FFFF : 0xFF0000FF);
					}
				}

				tracker.ch_ctr[c]--;
				tracker.old_ctr[c] = ctr;
			}
		} else {
			for(int c = 0; c < status->channels; c++) {
				Printf(36 + c * 64, 13, WHITE, "Ch %d", c + 1);
				Printf(36 + c * 64, 23, 0xFF808080, "-", c + 1);
			}
		}

		for(int i = 0; i < S_WIDTH * 8; i++) {
			PIXEL(i, MIDDLEROW_Y) += 0x303030;
		}

		int row = tracker.row - (S_HEIGHT - 8 - TRACKERWIN_Y) / 16;
		int drawn = 0;

		for(int y = TRACKERWIN_Y; y < S_HEIGHT - 8; y += 8) {
			if(row >= 0 && row < 64) {
				drawn = 1;

				DrawRow(y, tracker.order, row, status);

			} else {
				if(!drawn) {
					if(tracker.order > 0)
						DrawRow(y, tracker.order - 1, row + 64, status);
				} else {
					if(tracker.order < status->orders - 1)
						DrawRow(y, tracker.order + 1, row - 64, status);
				}

				for(int i = 0; i < S_WIDTH * 8; i++) {
					PIXEL(i, y) &= 0x7FFFFFFF;
				}
			}

			row++;
		}

		SDL_FillRect(surface, &order, 0xFF222222);
		SDL_FillRect(surface, &orderbar, WHITE);

		Printf(TRACKERWIN_W + 5, 13, WHITE, "Order");
		Printf(TRACKERWIN_W + 5, 23, WHITE, "table");
		Printf(TRACKERWIN_W + 5, 33, WHITE, "%02X/%02X", tracker.order, status->orders);

		int order = tracker.order - (S_HEIGHT - 8 - TRACKERWIN_Y) / 16;

		for(int y = TRACKERWIN_Y; y < S_HEIGHT - 8; y += 8) {
			if(order >= 0 && order < status->orders) {
				Printf(TRACKERWIN_W + 5, y, (order == tracker.order) ? WHITE : 0xFF808080,
					"%02X:%02X", order, status->ordertable[order]);
			}

			order++;
		}

		SDL_FillRect(surface, &bar, 0xFFFFFFFF);

		tracker.update = 0;
	}
}
