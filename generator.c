#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "defs.h"

void* generator(void *gen){									//funkcia generatora beziaca v samostatnom vlakne
	gen_struct_t (*genstr) = gen;

	unsigned world_size = 0;

	pthread_mutex_lock(&(genstr->mutex));					//generovanie zacne az po prvom signal
	pthread_cond_wait(&(genstr->doGenerate), &(genstr->mutex));
	pthread_mutex_unlock(&(genstr->mutex));

	while(genstr->run_gen){
		pthread_mutex_lock(&(genstr->mutex));
		if((genstr->gen_pos+1) % GEN_BUFF_SIZE == genstr->read_pos )
			pthread_cond_wait(&(genstr->doGenerate), &(genstr->mutex));	//buffer je plny, cakaj na uvolnenie miesta

		char old[genstr->height][genstr->width];

		unsigned i=0;
		for(unsigned y = 0; y < genstr->height; y++)
			for(unsigned x = 0; x < genstr->width; x++)
				old[y][x] = *(genstr->cells[genstr->gen_pos] + i++);

		genstr->gen_pos = (genstr->gen_pos + 1) % GEN_BUFF_SIZE;

		for(unsigned y = 0; y < genstr->height; y++)
			for(int x = 0; x < genstr->width; x++){
				unsigned n = 0;
				for (int y1 = y - 1; y1 <= y + 1; y1++)		//spocita sa pocet zijucich buniek v okoli
					for (int x1 = x - 1; x1 <= x + 1; x1++)
						if (old[(y1 + genstr->height) % genstr->height][(x1 + genstr->width) % genstr->width])
							n++;

				if (old[y][x]) n--;
				*(genstr->cells[genstr->gen_pos] + x + (y*genstr->width)) = (n == 3 || (n == 2 && old[y][x]));	//pravidlo 3/23 - bunka prezije iba ak ma 2-3 susedov, vznikne pri 3 susedoch
			}
		pthread_mutex_unlock(&(genstr->mutex));
		pthread_cond_signal(&(genstr->doRead));
	}
}


void gen_init(gen_struct_t *genstr){						//pociatocny stav generatora a spustenie vlakna
	for(unsigned i=0; i<GEN_BUFF_SIZE; i++)
		genstr->cells[i] = NULL;

	pthread_mutex_init(&(genstr->mutex), NULL);
	pthread_cond_init(&(genstr->doRead), NULL);
	pthread_cond_init(&(genstr->doGenerate), NULL);

	genstr->run_gen = 1;
	pthread_create(&(genstr->thread), NULL, generator, genstr);
}


void gen_clear(gen_struct_t *genstr){						//znicenie generatora
	genstr->run_gen = 0;									//ukoncenie generovania
	pthread_cond_signal(&(genstr->doGenerate));				//ak generator caka na signal, treba ho uvolnit aby mohol skoncit
	pthread_join(genstr->thread, NULL);

	for(unsigned i=0; i<GEN_BUFF_SIZE; i++)
		free(genstr->cells[i]);

	pthread_cond_destroy(&(genstr->doRead));
	pthread_cond_destroy(&(genstr->doGenerate));
	pthread_mutex_destroy(&(genstr->mutex));
}


void gen_loadWorld(gen_struct_t *genstr, world_t *world){	//nacitanie noveho sveta a jeho rozmerov
	pthread_mutex_lock(&(genstr->mutex));
	for(unsigned i=0; i<GEN_BUFF_SIZE; i++)
		genstr->cells[i] = realloc(genstr->cells[i], world->w * world->h);

	genstr->gen_pos = 0;
	genstr->read_pos = 0;
	memcpy(genstr->cells[0], world->cells, world->w * world->h);
	genstr->width = world->w;
	genstr->height = world->h;

	pthread_mutex_unlock(&(genstr->mutex));
	pthread_cond_signal(&(genstr->doGenerate));				//ak bol buffer plny, generator zastal
}


void gen_read(gen_struct_t *genstr, world_t *world){		//dalsia generacia na zobrazenie
	pthread_mutex_lock(&(genstr->mutex));

	if(genstr->gen_pos == genstr->read_pos){				//buffer je prazdny
		pthread_cond_signal(&(genstr->doGenerate));
		pthread_cond_wait(&(genstr->doRead), &(genstr->mutex));
	}

	memcpy(world->cells, genstr->cells[genstr->read_pos], world->w * world->h);

	genstr->read_pos = (genstr->read_pos + 1) % GEN_BUFF_SIZE;

	pthread_mutex_unlock(&(genstr->mutex));
	pthread_cond_signal(&(genstr->doGenerate));

	world->generation++;
}

