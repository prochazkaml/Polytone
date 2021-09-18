#include <stdio.h>
#include <libgen.h>
#include "sdl.h"
#include "diskio.h"
#include "tracker.h"
#include "libs/lz4.h"
#include "libs/tinyfiledialogs.h"

#ifdef _WIN32
#endif

buffer_t *buffer = NULL;
char *lastname = NULL;

#define put(x) *(out++) = x

void Reset() {
	if(buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}

	DrawBG();
}

int InitPOL(char *filename, uint8_t *data) {
	if(buffer == NULL) {
		buffer = malloc(sizeof(buffer_t));
	}

	if(PTPlayer_UnpackFile(data, buffer)) {
		UpdateStatus("%s is invalid!", filename ? basename(filename) : "New file");
		Reset();
		return 2;
	}

	tracker.row = 0;
	tracker.order = 0;
	tracker.octave = 3;
	tracker.column = 0;
	tracker.channel = 0;
	tracker.update = 1;
	tracker.selected = 0;
	tracker.s = PTPlayer_GetStatus();

	UpdateStatus("%s loaded!", filename ? basename(filename) : "New file");
	return 0;
}

int LoadPOL(char *filename) {
	/*
	 * Return values:
	 * 0: OK
	 * 1: Could not open file
	 * 2: File contents invalid
	 */

	printf("Input filename: \"%s\" -", filename);

	for(int i = 0; i < strlen(filename); i++) {
		printf(" %02X", filename[i]);
	}

	putchar('\n');

#ifdef _WIN32
	FILE *pt = _wfopen(tinyfd_utf8to16(filename), L"rb");
#else
	FILE *pt = fopen(filename, "rb");
#endif

	if(pt == NULL) {
		UpdateStatus("Could not open %s!", basename(filename));
		Reset();
		return 1;
	}

	if(buffer == NULL) {
		buffer = malloc(sizeof(buffer_t));
	}

	fseek(pt, 0, SEEK_END);
	int size = ftell(pt);
	fseek(pt, 0, SEEK_SET);

	uint8_t *data = malloc(size);

	fread(data, 1, size, pt);
	fclose(pt);

	lastname = filename;

	int ret = InitPOL(filename, data);

	free(data);

	return ret;
}

int SavePOL(char *filename) {
	/*
	 * Return values:
	 * 0: OK
	 * 1: Could not save file
	 */

	printf("Output filename: \"%s\" -", filename);

	for(int i = 0; i < strlen(filename); i++) {
		printf(" %02X", filename[i]);
	}

	putchar('\n');

#ifdef _WIN32
	FILE *pt = _wfopen(tinyfd_utf8to16(filename), L"wb");
#else
	FILE *pt = fopen(filename, "wb");
#endif

	if(pt == NULL) {
		UpdateStatus("Could not save %s!", basename(filename));
		return 1;
	}

	uint8_t *data = malloc(MAX_PT_SIZE);
	uint8_t *out = data + 0xD + buffer->orders;

	uint8_t oldeff[16];

	// Calculate the number of patterns

	int pats = 0;

	for(int i = 0; i < buffer->orders; i++) {
		if(buffer->ordertable[i] > pats) pats = buffer->ordertable[i];
	}

	// Generate a valid header

	memcpy(data, "\x08POLYTONE\x01", 10);

	data[0xA] = ++pats;
	data[0xB] = buffer->channels;
	data[0xC] = buffer->orders;

	memcpy(data + 0xD, buffer->ordertable, buffer->orders);

	// Pack the patterns

	for(int p = 0; p < pats; p++) {
		memset(oldeff, 0, sizeof(oldeff));

		// Analyze the pattern first

		int lastrow = -1;

		for(int r = 0; r < 64; r++) {
			for(int c = 0; c < buffer->channels; c++) {
				if(buffer->data[p][r][c].note || buffer->data[p][r][c].effect ||
					buffer->data[p][r][c].effectval) lastrow = r;
			}
		}

		if(lastrow >= 0) {
			for(int r = 0; r <= lastrow; r++) {
				// Analyze the row first

				int lastch = -1;

				for(int c = 0; c < buffer->channels; c++) {
					if(buffer->data[p][r][c].note || buffer->data[p][r][c].effect ||
						buffer->data[p][r][c].effectval) lastch = c;
				}

				if(lastch >= 0) {
					put((r == lastrow) ? (r | 0x80) : r);

					for(int c = 0; c <= lastch; c++) {
						unpacked_t d = buffer->data[p][r][c];

						if(d.note || d.effect || d.effectval) {
							uint8_t i = c;

							if(c == lastch) i |= 0x80;
							if(d.note) i |= 0x40;
							if(d.effect || d.effectval) i |= 0x20;

							put(i);

							if(d.note) {
								put(d.note);
							}

							if(d.effect || d.effectval) {
								i = d.effect;

								if(d.effectval == oldeff[c]) {
									i |= 0xE0;
									put(i);
								} else if(d.effectval >= 0xE) {
									i |= 0xF0;
									put(i);
									put(d.effectval);
								} else {
									i |= d.effectval << 4;
									put(i);
								}

								oldeff[c] = d.effectval;
							}
						}
					}
				}
			}
		} else {
			put(0xC0);
		}
	}

	fwrite(data, 1, out - data, pt);
	fclose(pt);
	free(data);

	lastname = filename;

	UpdateStatus("%s saved!", basename(filename));
	return 0;
}