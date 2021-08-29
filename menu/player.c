#include "player.h"
#include <SDL2/SDL.h>
#include "../MTPlayer/mtplayer.h"
#include "../diskio.h"
#include "../tracker.h"

menu_t submenu_player = {
	C(6), C(1), C(20), C(3), 3,
	BLACK, WHITE, {
	
	{ 0, C(0), C(20), C(1), "Resume/Pause    [F5]" },
	{ 0, C(1), C(20), C(1), "Play from start [F6]" },
	{ 0, C(2), C(20), C(1), "Stop            [F8]" },
}};

void (*submenu_player_fn[])() = {
	player_resume_pause, player_restart, player_stop
};

void player_resume_pause() {
	if(raw_mt != NULL) {
		if(SDL_GetAudioStatus() != SDL_AUDIO_PLAYING) {
			SDL_PauseAudio(0);
		} else {
			SDL_PauseAudio(1);
			UpdateStatus("Paused.");
		}
	} else {
		UpdateStatus("There is nothing to play.");
	}
}

void player_restart() {
	if(raw_mt != NULL) {
		SDL_PauseAudio(1);
		MTPlayer_Init(raw_mt);
		SDL_PauseAudio(0);
	} else {
		UpdateStatus("There is nothing to restart.");
	}
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