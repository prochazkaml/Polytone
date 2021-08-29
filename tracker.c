#include "tracker.h"
#include "sdl.h"
#include "MTPlayer/mtplayer.h"

tracker_t tracker;

void ParseKey(int mod, int scancode) {
	songstatus_t *status = MTPlayer_GetStatus();

	printf("%d %d\n", mod, scancode);

	if(SDL_GetAudioStatus() != SDL_AUDIO_PLAYING) {
		// Parse editor keys
		
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
			Printf(36 + c * 64, y, 0xFF777777, "---");
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
			Printf(68 + c * 64, y, 0xFF777777, "---");
		}
	}
}

#define TRACKERWIN_Y 48
#define BARWIDTH 2

int ch_ctr[12] = { 0 };
int old_ctr[12] = { 0 };

void RenderTracker() {
	songstatus_t *status = MTPlayer_GetStatus();
	SDL_Rect bar = { .x = 0, .y = TRACKERWIN_Y - BARWIDTH - 1, .w = S_WIDTH, .h = BARWIDTH};
	SDL_Rect rect = { .x = 0, .y = 8, .w = S_WIDTH, .h = S_HEIGHT - 16 };
	int rowy = (S_HEIGHT - 8 - TRACKERWIN_Y) / 16 * 8 + TRACKERWIN_Y;

	if(tracker.update) {
		SDL_FillRect(surface, &rect, 0xFF222222);

		rect.x = 32;
		rect.w = 64;

		while(rect.x < S_WIDTH) {
			SDL_FillRect(surface, &rect, 0xFF333333);

			rect.x += 128;
		}

		Printf(8, 22, WHITE, "%02X", status->ordertable[status->order]);

		SDL_FillRect(surface, &bar, 0xFFFFFFFF);

		if(SDL_GetAudioStatus() == SDL_AUDIO_PLAYING) {
			UpdateStatus("Playing %d.%02ds... (order %d/%d, speed %d)",
				samples / 100, samples % 100, status->order + 1, status->orders, status->tempo);

			for(int c = 0; c < status->channels; c++) {
				int color = status->channel[c].enabled ? WHITE : 0xFF808080;
				Printf(36 + c * 64, 12, color, "Ch %d", c + 1);
				Printf(36 + c * 64, 22, color, status->channel[c].active ? "%d Hz" : "-", status->channel[c].freq);

				int ctr = status->channel[c].ctr;

				if(ctr != old_ctr[c] && ch_ctr[c] < 14) ch_ctr[c] = 14;

				if(ch_ctr[c] < 0) ch_ctr[c] = 0;

				if(ctr < old_ctr[c]) ch_ctr[c] = 27;

				for(int x = 0; x < ch_ctr[c]; x++) {
					for(int y = 0; y < 8; y++) {
						PIXEL(37 + c * 64 + x * 2, 32 + y) = 
							(x < 14) ? 0xFF00FF00 : ((x < 21) ? 0xFF00FFFF : 0xFF0000FF);
					}
				}

				ch_ctr[c]--;
				old_ctr[c] = ctr;
			}
		} else {
			for(int c = 0; c < status->channels; c++) {
				Printf(36 + c * 64, bar.y - 8, WHITE, "Ch %d", c + 1);
			}
		}

		for(int i = 0; i < S_WIDTH * 8; i++) {
			PIXEL(i, rowy) += 0x303030;
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

		tracker.update = 0;
	}
}
