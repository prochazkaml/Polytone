#include <stdint.h>

typedef struct {
	int row, order, channel, column, octave, update;
	int selected, selrow, selchannel, selcolumn;
	int _selrow0, _selrow1, _selchannel0, _selchannel1, _selcolumn0, _selcolumn1;
	int8_t ch_ctr[12];
	int old_ctr[12];
} tracker_t;

extern tracker_t tracker;

void MoveCursor(int amount);
void InsertNote(int note);
void StopSelection();
void ParseKey(int mod, int scancode);
void RenderTracker();
