#include "tracker.h"
#include "sdl.h"
#include "MTPlayer/mtplayer.h"

tracker_t tracker;

void ParseKey(int mod, int scancode) {
	if(SDL_GetAudioStatus() != SDL_AUDIO_PLAYING) {
		tracker.update = 1;
	}
}

const char *notes[12] = {
	"C-", "C#", "D-", "D#", "E-", "F-",
	"F#", "G-", "G#", "A-", "A#", "B-"
};

const char effects[8] = "01234BDF";

#define TRACKERWIN_Y 8

void RenderTracker() {
	songstatus_t *status = MTPlayer_GetStatus();
	SDL_Rect rect = { .x = 0, .y = 8, .w = S_WIDTH, .h = S_HEIGHT - 16 };

	if(tracker.update) {
		SDL_FillRect(surface, &rect, 0xFF222222);

		rect.y = (S_HEIGHT - 8 - TRACKERWIN_Y) / 16 * 8 + TRACKERWIN_Y;
		rect.h = 8;

		SDL_FillRect(surface, &rect, 0xFF505050);

		int row = tracker.row - (S_HEIGHT - 8 - TRACKERWIN_Y) / 16;

		for(int y = TRACKERWIN_Y; y < S_HEIGHT - 8; y += 8) {
			if(row >= 0 && row < 64) {
				Printf(8, y, WHITE, "%02X", row);
			}

			row++;
		}

		tracker.update = 0;
	}
}
