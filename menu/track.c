#include "track.h"

menu_t submenu_track = {
	C(18), C(1), C(24), C(3), 3,
	BLACK, WHITE, {
	
	{ 0, C(0), C(24), C(1), "Add order entry    [F11]" },
	{ 0, C(1), C(24), C(1), "Remove order entry [F12]" },
	{ 0, C(2), C(24), C(1), "Add/remove channels" },
}};

void (*submenu_track_fn[])() = {
	NULL, NULL, NULL
};

