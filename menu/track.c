#include "track.h"
#include "player.h"
#include "../tracker.h"
#include "../diskio.h"
#include "../MTPlayer/mtplayer.h"

menu_t submenu_track = {
	C(18), C(1), C(24), C(3), 3,
	BLACK, WHITE, {
	
	{ 0, C(0), C(24), C(1), "Add order entry    [F11]" },
	{ 0, C(1), C(24), C(1), "Remove order entry [F12]" },
	{ 0, C(2), C(24), C(1), "Add/remove channels" },
}};

void (*submenu_track_fn[])() = {
	track_add_order, track_remove_order, NULL
};

void track_add_order() {
	int i;

	player_stop();

	for(i = 0xFF; i > tracker.order + 1; i--) {
		raw_mt[0x5F + i] = raw_mt[0x5E + i];
	}

	raw_mt[0x5F + i] = 0;

	MTPlayer_Init(raw_mt);
	MoveCursor(64);
	tracker.update = 1;
}

void track_remove_order() {
	int i;

	player_stop();

	for(i = tracker.order; i < 0xFF; i++) {
		raw_mt[0x5F + i] = raw_mt[0x60 + i];
	}

	raw_mt[0x15E] = 0xFF;

	MTPlayer_Init(raw_mt);
	if(tracker.order) MoveCursor(-64);
	tracker.update = 1;
}

