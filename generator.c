#include <stdlib.h>
#include <lpthread.h>
#include "defs.h"

void gen_init(void *gen){
	gen_struct_t (*genstr) = gen;

	for(unsigned i=0; i<GEN_BUFF_SIZE; i++)
		genstr->cells[i] = NULL;

	pthread_mutex_init(&(genstr->mutex), NULL);
	pthread_cond_init(&(genstr->doRead), NULL);
	pthread_cond_init(&(genstr->doGenerate), NULL);

	pthread_create(&(genstr->thread), NULL, generator, genstr);
}


void gen_clear(void *gen){
	gen_struct_t (*genstr) = gen;

	for(unsigned i=0; i<GEN_BUFF_SIZE; i++)
		free(genstr->cells[i]);

	//stop
	pthread_join(&(genstr->thread), NULL);
}


void gen_loadWorld(void *gen, void *wrld){
	/*
	 * lock
	 * realloc
	 * pos=0
	 * load
	 * unlock
	 * signal(gen)
	 * 
	 */
	//nacitanie rozmerov, pos=0, alokacia?
	
}


void gen_read(void *gen, void *wrld){
	/*
	 * lock
	 * if(empty)
	 * 		signal(gen)
	 * wait(read)
	 * 
	 * read
	 * pos++
	 * 
	 * unlock
	 * signal(gen)
	 *
	 */
}


void *generator(void *){
	/*
	 * lock
	 * if(full)
	 * 		wait(signal)
	 * evolve
	 * pos++
	 * unlock
	 * signal(read)
	 */
	//wait, evolve, signal
}
