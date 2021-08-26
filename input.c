#include "input.h"
#include "menu.h"
#include "sdl.h"

int mousex = 0, mousey = S_HEIGHT - 1;
int tbopen = -1, tbsel = -1, e;

void ParseInput() {
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				exit(0);
//					break;		// I mean...

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