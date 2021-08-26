#include "menu.h"

void (*submenu_file_fn[])() = {
	NULL, NULL, NULL, NULL, NULL, NULL
};

void (*submenu_player_fn[])() = {
	NULL, NULL, NULL
};

void (*submenu_edit_fn[])() = {
	NULL, NULL, NULL, NULL, NULL, NULL
};

void (*submenu_track_fn[])() = {
	NULL, NULL, NULL
};

void (*submenu_help_fn[])() = {
	NULL, NULL
};

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

menu_t submenu_file = {
	C(1), C(1), C(16), C(7), 7,
	BLACK, WHITE, {
	
	{ 0, C(0), C(16), C(1), "New     [Ctrl+n]" },
	{ 0, C(1), C(16), C(1), "Open    [Ctrl+o]" },
	{ 0, C(2), C(16), C(1), "Save    [Ctrl+s]" },
	{ 0, C(3), C(16), C(1), "Save as [Ctrl+S]" },
	{ 0, C(4), C(16), C(1), "Export  [Ctrl+e]" },
	{ 0, C(5), C(16), C(1), "Setup   [Ctrl+,]" },
	{ 0, C(6), C(16), C(1), "Quit    [Ctrl+q]" },
}};

menu_t submenu_player = {
	C(6), C(1), C(20), C(3), 3,
	BLACK, WHITE, {
	
	{ 0, C(0), C(20), C(1), "Resume/Pause    [F5]" },
	{ 0, C(1), C(20), C(1), "Play from start [F6]" },
	{ 0, C(2), C(20), C(1), "Stop            [F8]" },
}};

menu_t submenu_edit = {
	C(13), C(1), C(19), C(6), 6,
	BLACK, WHITE, {
	
	{ 0, C(0), C(19), C(1), "Cut        [Ctrl+x]" },
	{ 0, C(1), C(19), C(1), "Copy       [Ctrl+c]" },
	{ 0, C(2), C(19), C(1), "Paste      [Ctrl+v]" },
	{ 0, C(3), C(19), C(1), "Select all [Ctrl+a]" },
	{ 0, C(4), C(25), C(1), "Octave down    [F9]" },
	{ 0, C(5), C(25), C(1), "Octave up     [F10]" },
}};

menu_t submenu_track = {
	C(18), C(1), C(24), C(3), 3,
	BLACK, WHITE, {
	
	{ 0, C(0), C(24), C(1), "Add order entry    [F11]" },
	{ 0, C(1), C(24), C(1), "Remove order entry [F12]" },
	{ 0, C(2), C(24), C(1), "Add/remove channels" },
}};

menu_t submenu_help = {
	C(24), C(1), C(14), C(2), 2,
	BLACK, WHITE, {
	
	{ 0, C(0), C(14), C(1), "Manual    [F1]" },
	{ 0, C(1), C(14), C(1), "About Polytone" },
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
