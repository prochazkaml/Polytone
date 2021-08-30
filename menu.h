#ifndef menu_h	// This file would included many times at once when compiling menu.o
#define menu_h

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

enum dialogtype {
	DIALOG_SIMPLE,
	DIALOG_NUMBERINPUT,
	DIALOG_SCROLLABLE
};

typedef struct {
	enum dialogtype type;
	char *text;
	int buttons;
	char *button[];
} dialog_t;

typedef struct {
	dialog_t *dialog;
	int w, h;
	int bx, by;
	int nx, ny;
	uint8_t cur, min, max;
} dialogrender_t;

#define C(x) ((x) * 8)

extern menu_t topbar, *submenus[];

int CheckMenuBoundaries(int mousex, int mousey, menu_t *menu);
void DrawMenu(SDL_Surface *surface, menu_t *menu);
void SelectMenuItem(int category, int item);
int DrawDialog(dialog_t *dialog);

#endif