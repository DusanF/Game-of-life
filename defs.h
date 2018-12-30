#ifndef _defs
#define _defs


enum {
	GAME_STATE_RUN, GAME_STATE_PAUSE, GAME_STATE_STEP, GAME_STATE_EDIT
};

typedef struct{
	int w;
	int h;
	char* cells;
	unsigned generation;
	char state;
} world_t;

#endif
