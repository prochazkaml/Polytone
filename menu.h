#include <stdint.h>
#include "sdl.h"

typedef struct {
	int x, y, w, h;
	char *name;
} menuentry_t;

typedef struct {
	int x, y, w, h, entries;
	uint32_t fg, bg;
	menuentry_t entry[];
} menu_t;

#define C(x) (x * 8)

extern menu_t topbar, *submenus[];

int CheckMenuBoundaries(int mousex, int mousey, menu_t *menu);
void DrawMenu(SDL_Surface *surface, menu_t *menu);
void SelectMenuItem(int category, int item);
