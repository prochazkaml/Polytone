#include "player.h"

menu_t submenu_player = {
	C(6), C(1), C(20), C(3), 3,
	BLACK, WHITE, {
	
	{ 0, C(0), C(20), C(1), "Resume/Pause    [F5]" },
	{ 0, C(1), C(20), C(1), "Play from start [F6]" },
	{ 0, C(2), C(20), C(1), "Stop            [F8]" },
}};

void (*submenu_player_fn[])() = {
	NULL, NULL, NULL
};

