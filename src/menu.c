#include "menu.h"
#include <string.h>
#include "input.h"
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
	0, 0, -1, C(1), 5, 
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

	int width = (menu->w == -1) ? S_WIDTH : menu->w;

	if(mousex >= menu->x && mousex < (menu->x + width) &&
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

	if(menu->w == -1) rect.w = S_WIDTH;

	SDL_FillRect(surface, &rect, menu->bg);

	for(int e = 0; e < menu->entries; e++) {
		entry = menu->entry[e];

		entry.x += menu->x;
		entry.y += menu->y;

		_Printf(surface, entry.x, entry.y, menu->fg, "%s", entry.name);
	}
}

void SelectMenuItem(int category, int item) {
	static dialog_t na = {
		DIALOG_SIMPLE,
		"This function is not available yet",
		NULL, 1, { "Ok" }
	};
	
	if(submenus_fn[category][item] != NULL)
		submenus_fn[category][item]();
	else
		DrawDialog(&na);
}

void _DrawWindow(int w, int h) {
	SDL_Rect rect = {
		.x = (S_WIDTH - w) / 2 - 4,
		.y = (S_HEIGHT - h) / 2 - 4,
		.w = w + 8,
		.h = h + 8
	};

	SDL_FillRect(subsurface, NULL, 0x80000000);

	SDL_FillRect(subsurface, &rect, 0xFFFFFFFF);

	rect.x += 2; rect.y += 2;
	rect.w -= 4; rect.h -= 4;

	SDL_FillRect(subsurface, &rect, 0xFF000000);

	rect.x += 2; rect.y += 2;
	rect.w -= 4; rect.h -= 4;

	SDL_FillRect(subsurface, &rect, 0xFFFFFFFF);
}

void _DrawButton(int x, int y, char *text) {
	SDL_Rect rect = {
		.x = x, .y = y - 4,
		.w = (strlen(text) + 2) * 8, .h = 16
	};

	SDL_FillRect(subsurface, &rect, BLACK);

	rect.x += 2;
	rect.y += 2;
	rect.w -= 4;
	rect.h -= 4;

	SDL_FillRect(subsurface, &rect, WHITE);

	_Printf(subsurface, x + 8, y, BLACK, "%s", text);
}

int DrawDialog(dialog_t *dialog) {
	int w = 0, h = 16, bw = 0, pw, ph, x, y, retval; // h = 16 => make space for button(s)
	char *text = malloc(strlen(dialog->text) + 1);
	dialogrender_t render = { .dialog = dialog };
	SDL_Rect outernumfield = { .w = 32 + 4, .h = 8 + 5 };
	SDL_Rect innernumfield = { .w = 32, .h = 8 + 1 };

	// Figure out the dialog box dimensions

	// First, by looking at the dialog text

	strcpy(text, dialog->text);

	char *line = strtok(text, "\n");

	while(line != NULL) {
		if(strlen(line) * 8 > w) w = strlen(line) * 8;
		h += 8;
		line = strtok(NULL, "\n");
	}

	// Then by looking at all of the buttons

	for(int i = 0; i < dialog->buttons; i++) {
		bw += (strlen(dialog->button[i]) + 3) * 8;
	}

	bw -= 8;

	if(bw > w) w = bw;

	x = (S_WIDTH - w) / 2;
	y = (S_HEIGHT - h) / 2;

	pw = w + 16;
	ph = h + 16;

	render.w = pw + 8;	// 8 = borders
	render.h = ph + 8;

	// Fade in

	SDL_FillRect(subsurface, NULL, 0x80000000);

	for(int i = 1; i <= 10; i++) {
		_DrawWindow(i * pw / 10, i * ph / 10);
		RenderFrame();
	}

	// Draw the contents

	strcpy(text, dialog->text);

	line = strtok(text, "\n");

	while(line != NULL) {
		// Check for number input field

		if(dialog->type == DIALOG_NUMBERINPUT) {
			char *tag = strstr(line, "%%%%");

			if(tag != NULL) {
				innernumfield.x = render.nx = (S_WIDTH - strlen(line) * 8) / 2 + C(tag - line);
				innernumfield.y = render.ny = y;
				innernumfield.y--;

				outernumfield.x = render.nx - 2;
				outernumfield.y = render.ny - 3;

				render.cur = ((dialog_numberparam_t *)dialog->params)->def;
				render.min = ((dialog_numberparam_t *)dialog->params)->min;
				render.max = ((dialog_numberparam_t *)dialog->params)->max;
			}
		}

		_Printf(subsurface, (S_WIDTH - strlen(line) * 8) / 2, y, BLACK, "%s", line);

		y += 8;

		line = strtok(NULL, "\n");
	}

	// Draw the buttons

	y += 8;
	x = (S_WIDTH - bw) / 2;

	render.bx = x;
	render.by = y;

	for(int i = 0; i < dialog->buttons; i++) {
		_DrawButton(x, y, dialog->button[i]);

		x += (strlen(dialog->button[i]) + 3) * 8;
	}

	// Wait for user input

	InitDialogInput();

	while(1) {
		if((retval = ParseDialogInput(&render)) >= 0) break;

		// Draw the number field, if necessary

		if(dialog->type == DIALOG_NUMBERINPUT) {
			SDL_FillRect(subsurface, &outernumfield, BLACK);
			SDL_FillRect(subsurface, &innernumfield, WHITE);
			_Printf(subsurface, innernumfield.x, innernumfield.y + 1, BLACK, "% 4d", render.cur);
		}

		RenderFrame();
	}

	// Fade out

	for(int i = 9; i >= 0; i--) {
		_DrawWindow(i * pw / 10, i * ph / 10);
		RenderFrame();
	}

	SDL_FillRect(subsurface, NULL, 0);
	free(text);

	// Set the return value for the number field

	if(dialog->type == DIALOG_NUMBERINPUT) {
		((dialog_numberparam_t *)dialog->params)->def = render.cur;
	}

	return retval;
}
