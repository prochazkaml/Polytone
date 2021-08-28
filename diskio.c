#include <stdio.h>
#include <libgen.h>
#include "sdl.h"
#include "diskio.h"
#include "tracker.h"
#include "MTPlayer/mtplayer.h"

uint8_t *raw_mt = NULL;

#define MAX_MT_SIZE (0x15F + 255 * 12 * 64 * 2)

int LoadMON(char *filename) {
	/*
	 * Return values:
	 * 0: OK
	 * 1: Could not open file
	 * 2: File contents invalid
	 */

	FILE *mt = fopen(filename, "rb");

	if(mt == NULL) {
		UpdateStatus("Could not open %s!", basename(filename));
		return 1;
	}

	if(raw_mt == NULL) {
		raw_mt = malloc(MAX_MT_SIZE);
	}

	fread(raw_mt, 1, MAX_MT_SIZE, mt);
	fclose(mt);

	if(!MTPlayer_Init(raw_mt)) {
		UpdateStatus("%s is invalid!", basename(filename));
		return 2;
	}

	tracker.row = 0;
	tracker.order = 0;
	tracker.octave = 3;
	tracker.column = 0;
	tracker.channel = 0;
	tracker.update = 1;

	UpdateStatus("%s loaded!", basename(filename));
	return 0;
}
