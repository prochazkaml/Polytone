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
	track_add_order, track_remove_order, track_change_channels
};

void track_add_order() {
	if(raw_mt == NULL) return;

	songstatus_t *status = MTPlayer_GetStatus();
	int i;

	if(status->orders == 0xFF) return;

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
	if(raw_mt == NULL) return;

	songstatus_t *status = MTPlayer_GetStatus();
	int i;

	if(status->orders == 1) return;

	player_stop();

	for(i = tracker.order; i < 0xFF; i++) {
		raw_mt[0x5F + i] = raw_mt[0x60 + i];
	}

	raw_mt[0x15E] = 0xFF;

	MTPlayer_Init(raw_mt);
	if(tracker.order) MoveCursor(-64);
	tracker.update = 1;
}

void track_change_channels() {
	songstatus_t *status = MTPlayer_GetStatus();

	if(raw_mt == NULL) return;

	dialog_numberparam_t params = { status->channels, 1, 12 };

	dialog_t dialog = {
		DIALOG_NUMBERINPUT,
		"New number of channels: %%%%\n \n"
		"Use the \x7F/\x80 arrow keys to select.\n \n"
		"Ranges from 1 to 12.",
		&params, 2, { "Ok", "Cancel" }
	};

	static dialog_t nothing = {
		DIALOG_SIMPLE,
		"No changes have been made.",
		NULL, 1, { "Ok" }
	};

	static dialog_t ok = {
		DIALOG_SIMPLE,
		"Changes were successful.",
		NULL, 1, { "Ok" }
	};

	if(!DrawDialog(&dialog)) {
		int channels = params.def, old = status->channels;

		if(channels == status->channels) {
			DrawDialog(&nothing);
		} else if(channels > status->channels) {
			player_stop();

			// Expand the pattern data, start from the end

			int src = 255 * 64 * old - 1, dest, diff = channels - old;

			for(dest = 255 * 64 * channels - 1 - diff; dest > 0; dest--, src--) {
				status->data[dest] = status->data[src];

				if(src % old == 0) {
					dest -= diff;

					// Clear the free space
					memset(status->data + dest, 0, diff * 2);
				}
			}

			raw_mt[0x5D] = channels;
			MTPlayer_Init(raw_mt);

			DrawDialog(&ok);
		} else if(channels < status->channels) {
			player_stop();

			// Shrink the pattern data, start from the start

			int src = 0, dest, diff = old - channels;

			for(dest = 0; dest < 255 * 64 * channels; dest++, src++) {
				if(dest % channels == 0 && dest != 0)
					src += diff;

				status->data[dest] = status->data[src];
			}

			raw_mt[0x5D] = channels;
			MTPlayer_Init(raw_mt);

			if(tracker.channel >= status->channels) 
				tracker.channel = status->channels - 1;

			DrawDialog(&ok);
		}
	}
}
