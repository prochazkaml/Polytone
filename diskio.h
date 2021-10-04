#include <stdio.h>
#include <stdint.h>
#include "PTPlayer/ptplayer.h"

#ifdef _WIN32
#define diskio_fopen(file, mode) _wfopen(tinyfd_utf8to16(file), L##mode)
#else
#define diskio_fopen(file, mode) fopen(file, mode)
#endif

#define fput16(i, f) fputc((i) & 0xFF, f); fputc(((i) >> 8) & 0xFF, f)
#define fput32(i, f) fput16((i), f); fput16((i) >> 16, f)

#define fput16_be(i, f) fputc(((i) >> 8) & 0xFF, f); fputc((i) & 0xFF, f)
#define fput32_be(i, f) fput16_be((i) >> 16, f); fput16_be((i), f)

#define fputvlq(i, f) \
	if((i) >= 0x200000) fputc(((i) >> 21) & 0x7F | 0x80, f); \
	if((i) >= 0x4000) fputc(((i) >> 14) & 0x7F | 0x80, f); \
	if((i) >= 0x80) fputc(((i) >> 7) & 0x7F | 0x80, f); \
	fputc((i) & 0x7F, f)

extern buffer_t *buffer;
extern char *lastname;

int InitPOL(char *filename, uint8_t *data);
int LoadPOL(char *filename);
int SavePOL(char *filename);
