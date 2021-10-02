#include <stdio.h>
#include <stdint.h>
#include "PTPlayer/ptplayer.h"

#ifdef _WIN32
#define diskio_fopen(file, mode) _wfopen(tinyfd_utf8to16(file), L##mode)
#else
#define diskio_fopen(file, mode) fopen(file, mode)
#endif

#define fput16(i, f) fputc((i) & 0xFF, f); fputc(((i) & 0xFF00) >> 8, f)
#define fput32(i, f) fput16((i), f); fput16((i) >> 16, f)

extern buffer_t *buffer;
extern char *lastname;

int InitPOL(char *filename, uint8_t *data);
int LoadPOL(char *filename);
int SavePOL(char *filename);
