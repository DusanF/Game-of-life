#include <stdlib.h>
#include <string.h>
#include "defs.h"


unsigned hist_push(hist_t *history, world_t *world){		//Vlozenie stavu do historie
	history->hist_pos++;
	history->hist_pos = history->hist_pos % HIST_SIZE;

    history->cells[history->hist_pos] = realloc(history->cells[history->hist_pos], world->w*world->h);

	memcpy(history->cells[history->hist_pos], world->cells, world->w*world->h);

	if(history->hist_avail < HIST_SIZE)
		history->hist_avail++;

	return history->hist_avail;
}

unsigned hist_pop(hist_t *history, world_t *world){			//Navrat o jednu generaciu spat
	if(history->hist_avail == 0)							//Ak historia viac dozadu nesiaha, nerob nic
		return 0;

	memcpy(world->cells, history->cells[history->hist_pos], world->w*world->h);
	world->generation--;

	free(history->cells[history->hist_pos]);
	history->cells[history->hist_pos] = NULL;

	history->hist_pos = (history->hist_pos + HIST_SIZE - 1) % HIST_SIZE;
	history->hist_avail--;

	return history->hist_avail;
}

void hist_clear(hist_t* history){							//Vycistenie historie
	history->hist_pos = HIST_SIZE - 1;
	history->hist_avail = 0;

	for(unsigned i=0; i< HIST_SIZE; i++){
		free(history->cells[i]);
		history->cells[i] = NULL;
	}
}

void hist_init(hist_t *history){							//Inicializacia
	history->hist_pos = HIST_SIZE - 1;
	history->hist_avail = 0;

	for(unsigned i=0; i< HIST_SIZE; i++)
		history->cells[i] = NULL;
}
