#include <stdio.h>
#include "sdl.h"
#include "assets/font.h"
#include "assets/intro.h"
#include "assets/icon.h"
#include "libs/lz4.h"
#include "menu.h"
#include "diskio.h"
#include "tracker.h"

SDL_Window *window;
SDL_Surface *surface, *subsurface;
SDL_Renderer *renderer;
uint32_t *screen;
int samples, playing = 0, immnote, immnotedelay = 0, stopdelay = 0;

int VIDEO_SCALE = 2;

SDL_AudioSpec sdl_audio = {
	.freq = 44100,
	.format = AUDIO_S16,
	.channels = 1,
	.samples = 1024
};

void audio_callback(void *data, uint8_t *stream, int len) {
	memset(stream, 0, len);

	if(playing) {
		samples = PTPlayer_PlayInt16((int16_t *)stream, len / 2, sdl_audio.freq) / (sdl_audio.freq / 100);

		tracker.row = tracker.s->row;
		tracker.order = tracker.s->order;
		tracker.update = 1;
	}

	if(stopdelay > 0 && !--stopdelay) {
		playing = 0;
	}

	if(immnotedelay) {
		PTPlayer_PlayNoteInt16(immnote, (int16_t *)stream, len / 2, sdl_audio.freq);
		immnotedelay--;
	}
}

void Init() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    window = SDL_CreateWindow("Polytone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, S_WIDTH * VIDEO_SCALE, S_HEIGHT * VIDEO_SCALE, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_Surface *icon_surface = NewSurface(128, 128);
	lz4_uncompress(icon, icon_surface->pixels);
	SDL_SetWindowIcon(window, icon_surface);
	SDL_FreeSurface(icon_surface);

	SDL_RenderSetLogicalSize(renderer, S_WIDTH, S_HEIGHT);
	surface = NewSurface(S_WIDTH, S_HEIGHT);
	subsurface = NewSurface(S_WIDTH, S_HEIGHT);

	screen = (uint32_t *)surface->pixels;

	SDL_FillRect(subsurface, NULL, 0);

	sdl_audio.callback = audio_callback;
	SDL_OpenAudio(&sdl_audio, 0);

	SDL_PauseAudio(0);

	atexit(SDL_Quit);

	DrawBG();

	DrawMenu(surface, &topbar);
	UpdateStatus("Welcome to Polytone!");
}

void DrawBG() {
	lz4_uncompress(intro, screen + S_WIDTH * 8);
}

void RenderSurface(SDL_Surface *surface, int x, int y) {
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = surface->w;
	dst.h = surface->h;

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, &dst);
	SDL_DestroyTexture(texture);
}

SDL_Surface *NewSurface(int w, int h) {
	return SDL_CreateRGBSurface(0, w, h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
}

void _Putchar(SDL_Surface *surface, uint8_t c, int x, int y, uint32_t color) {
	uint32_t *screen = (uint32_t*)surface->pixels;

	for(int j = 0; j < 8; j++) {
		for(int i = 0; i < 8; i++) {
			if((font[(c - ' ') * 8 + j] >> (7 - i)) & 1) PIXEL(i + x, j + y) = color;
		}
	}
}

void _Printf(SDL_Surface *surface, int x, int y, uint32_t color, const char *format, ...) {
	char string[MAX_STR_LEN], c;
	int i = 0;
    
	va_list args;
	va_start(args, format);

	vsnprintf(string, MAX_STR_LEN, format, args);

	while(c = string[i++]) {
		_Putchar(surface, c, x, y, color);
		x += 8;
	}

    va_end(args);
}

void Clear(int x, int y, uint32_t bg, int px) {
	SDL_Rect rect = { .x = x, .y = y, .w = px, .h = 8 };

	SDL_FillRect(surface, &rect, bg);
}

void _Invert(SDL_Surface *surface, int x, int y, int h, uint32_t fg, uint32_t bg, int px) {
	uint32_t mask = ~(fg & bg);
	uint32_t *screen = (uint32_t*)surface->pixels;

	for(int j = y; j < y + h; j++) {
		for(int i = x; i < x + px; i++) {
			PIXEL(i, j) ^= mask;
		}
	}
}

void RenderFrame() {
	if(buffer != NULL) RenderTracker();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	RenderSurface(surface, 0, 0);
	RenderSurface(subsurface, 0, 0);

	SDL_RenderPresent(renderer);
}
