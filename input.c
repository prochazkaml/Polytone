#include "input.h"
#include <string.h>
#include "menu/file.h"
#include "menu/player.h"
#include "menu/edit.h"
#include "menu/track.h"
#include "menu/help.h"
#include "sdl.h"
#include "tracker.h"
#include "diskio.h"

typedef struct {
	SDL_Keymod mod;
	SDL_Scancode code;
	void (*fn)();
} kbdshortcut_t;

kbdshortcut_t shortcuts[] = {
	{ KMOD_LCTRL, SDL_SCANCODE_N, file_new },
	{ KMOD_LCTRL, SDL_SCANCODE_O, file_open },
	{ KMOD_LCTRL | KMOD_LSHIFT, SDL_SCANCODE_S, file_save_as },
	{ KMOD_LCTRL, SDL_SCANCODE_S, file_save },
	{ KMOD_LCTRL, SDL_SCANCODE_Q, file_quit },
	{ 0, SDL_SCANCODE_F5, player_resume_pause },
	{ 0, SDL_SCANCODE_F6, player_restart },
	{ 0, SDL_SCANCODE_F8, player_stop },
	{ 0, SDL_SCANCODE_F11, track_add_order },
	{ 0, SDL_SCANCODE_F12, track_remove_order },
};

#define n_shortcuts (sizeof(shortcuts) / sizeof(kbdshortcut_t))

int mousex = 0, mousey = S_HEIGHT - 1;
int tbopen = -1, tbsel = -1, e;

void ParseMainInput() {
	SDL_Event event;

	int i;
	
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				for(i = 0; i < n_shortcuts; i++) {
					if(((event.key.keysym.mod & shortcuts[i].mod) == shortcuts[i].mod ||
						((event.key.keysym.mod >> 1) & shortcuts[i].mod) == shortcuts[i].mod) &&
						event.key.keysym.scancode == shortcuts[i].code) {
						
						shortcuts[i].fn();

						break;
					}
				}

				if(i == n_shortcuts && raw_mt != NULL)
					ParseKey(event.key.keysym.mod, event.key.keysym.scancode);

				break;

			case SDL_QUIT:
				file_quit();				
				break;

			case SDL_MOUSEMOTION:
				if(event.motion.x && event.motion.x < S_WIDTH)
					mousex = event.motion.x;

				if(event.motion.y && event.motion.y < S_HEIGHT)
					mousey = event.motion.y;

				if(tbopen >= 0) {
					if((e = CheckMenuBoundaries(mousex, mousey, &topbar)) >= 0) {
						Invert(surface, topbar.entry[tbopen].x, topbar.entry[tbopen].y, WHITE, STATUS_BG, topbar.entry[tbopen].w);
						Invert(surface, topbar.entry[e].x, topbar.entry[e].y, WHITE, STATUS_BG, topbar.entry[e].w);

						SDL_FillRect(subsurface, NULL, 0);
						DrawMenu(subsurface, submenus[e]);

						tbopen = e;
					}

					if((e = CheckMenuBoundaries(mousex, mousey, submenus[tbopen])) >= 0) {
						if(e != tbsel) {
							if(tbsel >= 0) Invert(subsurface, 
								submenus[tbopen]->x + submenus[tbopen]->entry[tbsel].x,
								submenus[tbopen]->y +submenus[tbopen]->entry[tbsel].y,
								BLACK, WHITE,
								submenus[tbopen]->entry[tbsel].w);

							Invert(subsurface, 
								submenus[tbopen]->x + submenus[tbopen]->entry[e].x,
								submenus[tbopen]->y + submenus[tbopen]->entry[e].y,
								BLACK, WHITE,
								submenus[tbopen]->entry[e].w);

							tbsel = e;
						}
					} else {
						if(tbsel >= 0) {
							Invert(subsurface, 
								submenus[tbopen]->x + submenus[tbopen]->entry[tbsel].x,
								submenus[tbopen]->y +submenus[tbopen]->entry[tbsel].y,
								BLACK, WHITE,
								submenus[tbopen]->entry[tbsel].w);

							tbsel = -1;
						}
					}
				}

				break;

			case SDL_MOUSEBUTTONDOWN:
				if((e = CheckMenuBoundaries(mousex, mousey, &topbar)) >= 0) {
					Invert(surface, topbar.entry[e].x, topbar.entry[e].y, WHITE, STATUS_BG, topbar.entry[e].w);

					DrawMenu(subsurface, submenus[e]);

					tbopen = e;
					tbsel = -1;
				}

				break;

			case SDL_MOUSEBUTTONUP:
				if((e = tbopen) >= 0) {
					Invert(surface, topbar.entry[e].x, topbar.entry[e].y, WHITE, STATUS_BG, topbar.entry[e].w);

					SDL_FillRect(subsurface, NULL, 0);

					if(tbopen >= 0 && tbsel >= 0) {
						SelectMenuItem(tbopen, tbsel);
					}

					tbopen = -1;
					tbsel = -1;
				}

				break;
		}
	}
}

int selected, selectedx, selectedw;

void InitDialogInput() {
	SDL_Event event;

	event.type = SDL_MOUSEMOTION;
	event.motion.x = mousex;
	event.motion.y = mousey;

	SDL_PushEvent(&event);

	selected = -1;
}

int ParseDialogInput(dialogrender_t *dialog) {
	SDL_Event event;
	int i, bx, by, bx2, by2;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				return dialog->dialog->buttons - 1;

			case SDL_KEYDOWN:
				switch(event.key.keysym.scancode) {
					case SDL_SCANCODE_ESCAPE:
						return dialog->dialog->buttons - 1;

					case SDL_SCANCODE_RETURN:
					case SDL_SCANCODE_SPACE:
						return 0;

					case SDL_SCANCODE_UP:
						if(dialog->dialog->type == DIALOG_NUMBERINPUT &&
							dialog->cur < dialog->max) dialog->cur++;
						
						break;

					case SDL_SCANCODE_DOWN:
						if(dialog->dialog->type == DIALOG_NUMBERINPUT &&
							dialog->cur > dialog->min) dialog->cur--;
						
						break;

					case SDL_SCANCODE_HOME:
						if(dialog->dialog->type == DIALOG_NUMBERINPUT)
							dialog->cur = dialog->min;

						break;						

					case SDL_SCANCODE_END:
						if(dialog->dialog->type == DIALOG_NUMBERINPUT)
							dialog->cur = dialog->max;

						break;						
				}

				break;

			case SDL_MOUSEMOTION:
				if(event.motion.x && event.motion.x < S_WIDTH)
					mousex = event.motion.x;

				if(event.motion.y && event.motion.y < S_HEIGHT)
					mousey = event.motion.y;

				bx = dialog->bx, by = dialog->by - 4;
				by2 = by + 16;

				for(i = 0; i < dialog->dialog->buttons; i++) {
					bx2 = bx + (strlen(dialog->dialog->button[i]) + 2) * 8;

					if(mousex >= bx && mousex < bx2 && mousey >= by && mousey < by2) {
						if(i != selected) {
							if(selected >= 0) {
								_Invert(subsurface, selectedx, by + 2, 12, BLACK, WHITE, selectedw);
							}

							selectedx = bx + 2;
							selectedw = bx2 - bx - 4;
							selected = i;

							_Invert(subsurface, selectedx, by + 2, 12, BLACK, WHITE, selectedw);
						}

						break;
					}
					
					bx += (strlen(dialog->dialog->button[i]) + 3) * 8;
				}

				if(i == dialog->dialog->buttons && selected >= 0) {
					_Invert(subsurface, selectedx, by + 2, 12, BLACK, WHITE, selectedw);
					selected = -1;
				}

				break;

			case SDL_MOUSEBUTTONUP:
				if(mousex < (S_WIDTH - dialog->w) / 2 ||
				   mousex >= (S_WIDTH + dialog->w) / 2 ||
				   mousey < (S_HEIGHT - dialog->h) / 2 ||
				   mousey >= (S_HEIGHT + dialog->h) / 2)
					return dialog->dialog->buttons - 1;

				if((i = selected) >= 0) {
					return i;
				}

				break;
		}
	}

	return -1;
}
