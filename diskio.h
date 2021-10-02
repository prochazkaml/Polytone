#include <stdio.h>
#include <stdint.h>
#include "PTPlayer/ptplayer.h"

#ifdef _WIN32
#define diskio_fopen(file, mode) _wfopen(tinyfd_utf8to16(file), L##mode)
#else
#define diskio_fopen(file, mode) fopen(file, mode)
#endif

extern buffer_t *buffer;
extern char *lastname;

int InitPOL(char *filename, uint8_t *data);
int LoadPOL(char *filename);
int SavePOL(char *filename);
