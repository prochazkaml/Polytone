#include <stdint.h>
#define MAX_MT_SIZE (0x15F + 255 * 12 * 64 * 2)

extern uint8_t *raw_mt;
extern char *lastname;

int InitMON(char *filename);
int LoadMON(char *filename);
