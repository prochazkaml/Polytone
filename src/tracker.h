#include <stdint.h>
#include "libs/PTPlayer/ptplayer.h"

typedef struct {
	int row, order, channel, column, xscroll, octave, update;
	int selected, selrow, selchannel, selcolumn;
	int _selrow0, _selrow1, _selchannel0, _selchannel1, _selcolumn0, _selcolumn1;
	songstatus_t *s;
	int8_t ch_ctr[12];
	int old_ctr[12];
} tracker_t;

extern tracker_t tracker;

#define InsertNote(x) PutData(0, x)
#define InsertEffect(x) PutData(1, x)
#define InsertEffectValue(pos, x) PutData(2 + pos, x)

void MoveCursor(int amount);
void PutData(int col, int val);
int GetData(int col);
void StopSelection();
void CheckSelection(int mod);
void UpdateSelectedBox();
int DeleteSelectedBox();
void ParseKey(int mod, int scancode);
void RenderTracker();
