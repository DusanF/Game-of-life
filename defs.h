#ifndef _defs
#define _defs

#define for_cells for (int i=0; i<(world->w * world->h); i++)
#define for_x for(int x = 0; x < world->w; x++)
#define for_y for(int y = 0; y < world->h; y++)
#define for_xy for_x for_y
#define for_yx for_y for_x

enum {
	GAME_STATE_RUN, GAME_STATE_PAUSE, GAME_STATE_STEP
};

typedef struct{
	int w;
	int h;
	char* cells;
} world_t;

#endif
