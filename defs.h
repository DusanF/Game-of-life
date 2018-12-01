#ifndef _defs
#define _defs

#include <pthread.h>

#define for_x for (int x = 0; x < w; x++)
#define for_y for (int y = 0; y < h; y++)
#define for_xy for_x for_y
#define for_yx for_y for_x

enum {
	STAV_RUN, STAV_PAUSE, STAV_STEP
};

typedef struct Klavesa {
    int tlacidlo;
    int zmena;
    //pthread_cond_t stlacene;
    pthread_mutex_t kl_mut;
} KLAVESA;

#endif
