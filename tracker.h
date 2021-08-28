typedef struct {
	int row, order, channel, column, octave, update;
} tracker_t;

extern tracker_t tracker;

void ParseKey(int mod, int scancode);
void RenderTracker();
