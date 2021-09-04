#include <stdio.h>
#include <libgen.h>
#include "sdl.h"
#include "diskio.h"
#include "tracker.h"
#include "libs/lz4.h"

uint8_t *raw_mt = NULL;
char *lastname = NULL;

void Reset() {
	if(raw_mt != NULL) {
		free(raw_mt);
		raw_mt = NULL;
	}

	DrawBG();
}

int InitMON(char *filename) {
	if(!MTPlayer_Init(raw_mt)) {
		UpdateStatus("%s is invalid!", basename(filename));
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
	tracker.s = MTPlayer_GetStatus();

	UpdateStatus("%s loaded!", basename(filename));
	return 0;
}

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
		Reset();
		return 1;
	}

	if(raw_mt == NULL) {
		raw_mt = malloc(MAX_MT_SIZE);
	}

	memset(raw_mt, 0, MAX_MT_SIZE);

	fread(raw_mt, 1, MAX_MT_SIZE, mt);
	fclose(mt);

	lastname = filename;

	return InitMON(filename);
}

int SaveMON(char *filename) {
	/*
	 * Return values:
	 * 0: OK
	 * 1: Could not save file
	 */

	FILE *mt = fopen(filename, "wb");

	if(mt == NULL) {
		UpdateStatus("Could not save %s!", basename(filename));
		return 1;
	}

	// Calculate the number of patterns

	int pats = 0;

	for(int i = 0x5F; i < 0x15F; i++) {
		if(raw_mt[i] > pats && raw_mt[i] != 0xFF) pats = raw_mt[i];
	}

	raw_mt[0x5C] = ++pats;

	fwrite(raw_mt, 1, 0x15F + pats * 64 * tracker.s->channels * 2, mt);
	fclose(mt);

	lastname = filename;

	UpdateStatus("%s saved!", basename(filename));
	return 0;
}