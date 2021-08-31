#include "../menu.h"

extern menu_t submenu_track;
extern void (*submenu_track_fn[])();

void track_add_order();
void track_remove_order();
void track_change_channels();
