#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

void file_save(void *w){
	world_t (*world) = w;

	char *filename;
	FILE *file;
	char aktual=0;
	unsigned pocet=-1;

	filename = malloc(FILENAME_MAX);

    printf("Ulozit ako: ");
    scanf("%s", filename);

	file = fopen(filename, "w");
	if(file == 0){
		perror("Chyba pri vytvarani suboru");
		return;
		}

	fprintf(file, "%u\n%u\n", world->w, world->h);
	for(int i=0; i<(world->w * world->h); i++){
		pocet++;
		if(*(world->cells + i) != aktual){
			fprintf(file, "%u\n", pocet);
			pocet = 0;
			aktual = *(world->cells + i);
		}
	}
	fclose(file);
	free(filename);
}
