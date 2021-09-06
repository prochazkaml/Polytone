#include "track.h"
#include "player.h"
#include "../tracker.h"
#include "../diskio.h"

menu_t submenu_track = {
	C(18), C(1), C(24), C(3), 3,
	BLACK, WHITE, {
	
	{ 0, C(0), C(24), C(1), "Add order entry    [F12]" },
	{ 0, C(1), C(24), C(1), "Remove order entry [F11]" },
	{ 0, C(2), C(24), C(1), "Add/remove channels" },
}};

void (*submenu_track_fn[])() = {
	track_add_order, track_remove_order, track_change_channels
};

void track_add_order() {
	if(buffer == NULL) return;

	int i;

	if(buffer->orders == 0x100) return;

	player_stop();

	for(i = 0xFF; i > tracker.order + 1; i--) {
		buffer->ordertable[i] = buffer->ordertable[i - 1];
	}

	buffer->ordertable[i] = 0;
	buffer->orders++;

	PTPlayer_Reset(buffer);
	MoveCursor(64);
	tracker.update = 1;
}

void track_remove_order() {
	if(buffer == NULL) return;

	int i;

	if(buffer->orders == 1) return;

	player_stop();

	for(i = tracker.order; i < 0xFF; i++) {
		buffer->ordertable[i] = buffer->ordertable[i + 1];
	}

	buffer->orders--;

	PTPlayer_Reset(buffer);
	if(tracker.order) MoveCursor(-64);
	tracker.update = 1;
}

void track_change_channels() {
	if(buffer == NULL) return;

	dialog_numberparam_t params = { buffer->channels, 1, 12 };

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
		int channels = params.def;

		if(channels == buffer->channels) {
			DrawDialog(&nothing);
		} else if(channels > buffer->channels) {
			player_stop();

			// Expand the pattern data, start from the end

			for(int o = 0; o < 256; o++) {
				for(int r = 0; r < 64; r++) {
					unpacked_t *data = &buffer->data[o][r][channels];
					data->note = 0; data->effect = 0; data->effectval = 0;
				}
			}

			buffer->channels = channels;
			PTPlayer_Reset(buffer);

			DrawDialog(&ok);
		} else if(channels < buffer->channels) {
			player_stop();

			buffer->channels = channels;
			PTPlayer_Reset(buffer);

			if(tracker.channel >= buffer->channels) 
				tracker.channel = buffer->channels - 1;

			DrawDialog(&ok);
		}
	}
}
