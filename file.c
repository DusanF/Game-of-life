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
    while(fgets(filename, FILENAME_MAX, stdin) == NULL);

	file = fopen(filename, "w");
	free(filename);

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

void file_load(void *w){
	world_t (*world) = w;

	char *filename;
	FILE *file;
	char aktual=0;
	unsigned pocet, pos=0;
	
	filename = malloc(FILENAME_MAX);

    printf("Nazov suboru: ");
    while(fgets(filename, FILENAME_MAX, stdin) == NULL);

	file = fopen(filename, "r");
	free(filename);
	if(file == 0){
		perror("Chyba pri otvarani suboru");
		return;
		}

	fscanf(file, "%u", &(world->w));
	fscanf(file, "%u", &(world->h));
	world->cells = realloc(world->cells, world->h * world->w * sizeof(char));

	while(fscanf(file, "%u", &pocet) != EOF){
		while(pocet--){
			*(world->cells+pos) = aktual;
			pos++;
		}
		aktual = aktual ? 0 : 1;
	}
	while(pos < world->h * world->w){
		*(world->cells+pos) = aktual;
		pos++;
	}
	world->generation = 0;

	fclose(file);
}
