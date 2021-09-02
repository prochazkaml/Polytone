#include "player.h"
#include <SDL2/SDL.h>
#include "../MTPlayer/mtplayer.h"
#include "../diskio.h"
#include "../tracker.h"

menu_t submenu_player = {
	C(6), C(1), C(20), C(4), 4,
	BLACK, WHITE, {
	
	{ 0, C(0), C(20), C(1), "Resume/Pause    [F5]" },
	{ 0, C(1), C(20), C(1), "Play from start [F6]" },
	{ 0, C(2), C(20), C(1), "Play pattern    [F7]" },
	{ 0, C(3), C(20), C(1), "Stop            [F8]" },
}};

void (*submenu_player_fn[])() = {
	player_resume_pause, player_restart, player_pattern, player_stop
};

void player_resume_pause() {
	songstatus_t *status = MTPlayer_GetStatus();

	if(raw_mt != NULL) {
		if(SDL_GetAudioStatus() != SDL_AUDIO_PLAYING) {
			memset(tracker.old_ctr, 0, sizeof(tracker.old_ctr));
			memset(tracker.ch_ctr, 27, sizeof(tracker.ch_ctr));

			if(tracker.row != status->row || tracker.order != status->order) {
				MTPlayer_Init(raw_mt);

				status->order = tracker.order;
				status->row = tracker.row - 1;
			}

			SDL_PauseAudio(0);
		} else {
			SDL_PauseAudio(1);
			UpdateStatus("Paused.");
			tracker.update = 1;
		}
	} else {
		UpdateStatus("There is nothing to play.");
	}
}

void player_restart() {
	if(raw_mt != NULL) {
		SDL_PauseAudio(1);
		MTPlayer_Init(raw_mt);
		memset(tracker.old_ctr, 0, sizeof(tracker.old_ctr));
		memset(tracker.ch_ctr, 27, sizeof(tracker.ch_ctr));
		SDL_PauseAudio(0);
	} else {
		UpdateStatus("There is nothing to restart.");
	}
}

void player_pattern() {
	SDL_PauseAudio(1);

	tracker.row = 0;

	player_resume_pause();
}

void player_stop() {
	if(raw_mt != NULL) {
		SDL_PauseAudio(1);
		MTPlayer_Init(raw_mt);
		UpdateStatus("Stopped.");
	} else {
		UpdateStatus("There is nothing to stop.");
	}

	tracker.update = 1;
}