#include "player.h"
#include "../sdl.h"
#include "../diskio.h"
#include "../tracker.h"

menu_t submenu_player = {
	C(6), C(1), C(21), C(5), 5,
	BLACK, WHITE, {
	
	{ 0, C(0), C(21), C(1), "Play single line [F4]" },
	{ 0, C(1), C(21), C(1), "Resume/Pause     [F5]" },
	{ 0, C(2), C(21), C(1), "Play from start  [F6]" },
	{ 0, C(3), C(21), C(1), "Play pattern     [F7]" },
	{ 0, C(4), C(21), C(1), "Stop             [F8]" },
}};

void (*submenu_player_fn[])() = {
	player_play_line, player_resume_pause, player_restart, player_pattern, player_stop
};

void _play_from_line() {
	songstatus_t old;

	memset(tracker.old_ctr, 0, sizeof(tracker.old_ctr));
	memset(tracker.ch_ctr, 27, sizeof(tracker.ch_ctr));

	PTPlayer_Reset(buffer);

	memcpy(&old, tracker.s, sizeof(songstatus_t));
		
	int i, j;

	while(1) {
		if((i = tracker.s->order * 64 + tracker.s->row) == (tracker.order * 64 + tracker.row)) {
			memcpy(tracker.s, &old, sizeof(songstatus_t));
			break;
		}

		memcpy(&old, tracker.s, sizeof(songstatus_t));
		
		PTPlayer_ProcessTick();

		if(tracker.s->order * 64 + tracker.s->row != i) {
			j = 0;
		} else {
			j++;
		}

		if(j > 33 || tracker.s->order * 64 + tracker.s->row < i) {
			// Fallback if the tracker returned to the start
			// or if it got stuck on a single line

			PTPlayer_Reset(buffer);
			
			tracker.s->order = tracker.order;
			tracker.s->row = tracker.row - 1;

			break;
		}
	}
}

void player_play_line() {
	if(buffer != NULL) {
		if(!playing) {
			_play_from_line();

			stopdelay = 15;

			tracker.s->allowadvance = -1;
			playing = 1;
		}
	} else {
		UpdateStatus("There is nothing to play.");
	}
}

void player_resume_pause() {
	if(buffer != NULL) {
		if(!playing) {
			_play_from_line();

			playing = 1;
		} else {
			playing = 0;
			UpdateStatus("Paused.");
			tracker.update = 1;
		}
	} else {
		UpdateStatus("There is nothing to play.");
	}
}

void player_restart() {
	if(buffer != NULL) {
		playing = 0;
		tracker.row = 0;
		tracker.order = 0;

		player_resume_pause();
	} else {
		UpdateStatus("There is nothing to restart.");
	}
}

void player_pattern() {
	playing = 0;
	tracker.row = 0;

	player_resume_pause();
}

void player_stop() {
	if(buffer != NULL) {
		playing = 0;
		PTPlayer_Reset(buffer);
		UpdateStatus("Stopped.");
	} else {
		UpdateStatus("There is nothing to stop.");
	}

	tracker.update = 1;
}