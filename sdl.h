#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>

#define S_WIDTH 400
#define S_HEIGHT 240
#define MAX_STR_LEN (S_WIDTH / 8 - 1)

#define STATUS_Y (S_HEIGHT - 8)

#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000
#define STATUS_BG 0xFF606060

extern SDL_Surface *surface, *subsurface;
extern uint32_t *screen;

#define PIXEL(x, y) screen[(x) + (y) * S_WIDTH]

void Init();
void RenderSurface(SDL_Surface *surface, int x, int y);
SDL_Surface *NewSurface(int w, int h);
void RenderFrame();

void _Putchar(SDL_Surface *surface, uint8_t c, int x, int y, uint32_t color);
void _Printf(SDL_Surface *surface, int x, int y, uint32_t color, const char *format, ...);
void _Invert(SDL_Surface *surface, int x, int y, int h, uint32_t fg, uint32_t bg, int px);
void Clear(int x, int y, uint32_t bg, int px);

#define Invert(s, x, y, fg, bg, px)  _Invert(s, x, y, 8, fg, bg, px)
#define Putchar(c, x, y, color) _Putchar(surface, c, x, y, color)
#define Printf(x, y, color, ...) _Printf(surface, x, y, color, __VA_ARGS__)
#define UpdateStatus(...) \
	Clear(0, STATUS_Y, STATUS_BG, S_WIDTH); \
	Printf(8, STATUS_Y, WHITE, __VA_ARGS__)
