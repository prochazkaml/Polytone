#include "menu.h"
#include "menu/file.h"
#include "menu/player.h"
#include "menu/edit.h"
#include "menu/track.h"
#include "menu/help.h"

void (**submenus_fn[])() = {
	submenu_file_fn, submenu_player_fn, submenu_edit_fn,
	submenu_track_fn, submenu_help_fn
};

menu_t topbar = {
	0, 0, S_WIDTH, C(1), 5, 
	WHITE, STATUS_BG, {

	{ C(1), 0, C(4), C(1), "File" },
	{ C(6), 0, C(6), C(1), "Player" },
	{ C(13), 0, C(4), C(1), "Edit" },
	{ C(18), 0, C(5), C(1), "Track" },
	{ C(24), 0, C(4), C(1), "Help" }
}};

menu_t *submenus[] = {
	&submenu_file, &submenu_player, &submenu_edit,
	&submenu_track, &submenu_help
};

int CheckMenuBoundaries(int mousex, int mousey, menu_t *menu) {
	/* 
	 * Return values:
	 * -2 = cursor not in range
	 * -1 = cursor in range, but not over an element
	 * 0..entries-1 = Cursor over an element
	 */

	menuentry_t entry;

	if(mousex >= menu->x && mousex < (menu->x + menu->w) &&
	   mousey >= menu->y && mousey < (menu->y + menu->h)) {
		for(int e = 0; e < menu->entries; e++) {
			entry = menu->entry[e];

			entry.x += menu->x;
			entry.y += menu->y;

			if(mousex >= entry.x && mousex < (entry.x + entry.w) &&
			   mousey >= entry.y && mousey < (entry.y + entry.h))
				return e;
		}

		return -1;
	} else {
		return -2;
	}
}

void DrawMenu(SDL_Surface *surface, menu_t *menu) {
	menuentry_t entry;

	SDL_Rect rect = { .x = menu->x, .y = menu->y, .w = menu->w, .h = menu->h };

	SDL_FillRect(surface, &rect, menu->bg);

	for(int e = 0; e < menu->entries; e++) {
		entry = menu->entry[e];

		entry.x += menu->x;
		entry.y += menu->y;

		_Printf(surface, entry.x, entry.y, menu->fg, "%s", entry.name);
	}
}

void SelectMenuItem(int category, int item) {
	if(submenus_fn[category][item] != NULL)
		submenus_fn[category][item]();
}
