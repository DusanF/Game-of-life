#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

void file_save(world_t *world){
	char *filename;
	FILE *file;
	char aktual=0;

	filename = malloc(FILENAME_MAX);

    printf("Ulozit ako: ");
    while(fgets(filename, FILENAME_MAX-4, stdin) == NULL);
    *(filename+strcspn(filename, "\n")) = 0;
    
    if(strstr(filename, ".gol") == NULL)		//prida priponu, ak uz nieje zadana
		strcat(filename, ".gol");

	file = fopen(filename, "w");
	printf("Ukladam do \"%s\"...\n", filename);
	free(filename);

	if(file == 0){
		perror("Chyba pri vytvarani suboru");
		return;
		}

	unsigned pocet = 0;

	fprintf(file, "%u\n%u\n", world->w, world->h);
	for(int i=0; i<(world->w * world->h); i++){
		if(*(world->cells + i) != aktual){
			fprintf(file, "%u\n", pocet);
			pocet = 1;
			aktual = *(world->cells + i);
		} else
			pocet++;
	}
	fprintf(file, "%u\n", pocet);
	fclose(file);
}

void file_load(world_t *world){
	char *filename;
	FILE *file;
	char cell_stat=0;
	unsigned series, pos=0;
	
	filename = malloc(FILENAME_MAX);

    printf("Nazov suboru: ");
    while(fgets(filename, FILENAME_MAX-4, stdin) == NULL);
    *(filename+strcspn(filename, "\n")) = 0;

    if(strstr(filename, ".gol") == NULL)				//prida priponu, ak uz nieje zadana
		strcat(filename, ".gol");

	printf("Otvaram \"%s\"...\n", filename);
	file = fopen(filename, "r");
	free(filename);
	if(file == 0){
		perror("Chyba pri otvarani suboru");
		return;
		}

	fscanf(file, "%u", &(world->w));
	fscanf(file, "%u", &(world->h));
	world->cells = realloc(world->cells, world->h * world->w);

	while(fscanf(file, "%u", &series) != EOF){
		while(series--){
			*(world->cells+pos) = cell_stat;
			pos++;
		}
		cell_stat = cell_stat ? 0 : 1;
	}
	while(pos < world->h * world->w){
		*(world->cells+pos) = cell_stat;
		pos++;
	}
	world->generation = 0;

	fclose(file);
}
