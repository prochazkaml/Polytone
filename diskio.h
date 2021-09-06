#include <stdint.h>
#include "PTPlayer/ptplayer.h"

extern buffer_t *buffer;
extern char *lastname;

int InitPOL(char *filename, uint8_t *data);
int LoadPOL(char *filename);
int SavePOL(char *filename);
