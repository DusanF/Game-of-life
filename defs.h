#ifndef _defs
#define _defs


#define MIN_H 5
#define MAX_H 60
#define DEF_H 25
#define MIN_W 5
#define MAX_W 130
#define DEF_W 50

#define PORT 12345

enum {
	GAME_STATE_RUN, GAME_STATE_PAUSE, GAME_STATE_STEP, GAME_STATE_EDIT
};

enum {
	SERVER_SAVE_OK, SERVER_SAVE_ERR, SERVER_USER_NEW, SERVER_USER_OK
};

typedef struct{
	int w;
	int h;
	char* cells;
	unsigned generation;
	char state;
} world_t;

#endif
