#include "../menu.h"

extern menu_t submenu_player;
extern void (*submenu_player_fn[])();

void player_resume_pause();
void player_restart();
void player_stop();
