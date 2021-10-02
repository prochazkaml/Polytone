#include "../menu.h"

extern menu_t submenu_file;
extern void (*submenu_file_fn[])();

void file_new();
void file_open();
void file_save();
void file_save_as();
void file_export();
void file_quit();
