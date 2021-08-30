#include <stdint.h>

typedef struct {
	int row, order, channel, column, octave, update;
	int8_t ch_ctr[12];
	int old_ctr[12];
} tracker_t;

extern tracker_t tracker;

void ParseKey(int mod, int scancode);
void RenderTracker();
