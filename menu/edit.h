#include "../menu.h"

extern menu_t submenu_edit;
extern void (*submenu_edit_fn[])();

void edit_cut();
void edit_copy();
void edit_paste();
void edit_select_all();
void edit_octave_down();
void edit_octave_up();