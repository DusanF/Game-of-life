#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "defs.h"
#include "gui.h"

const unsigned dly_tab[] = {
	1000000,
	700000,
	400000,
	200000,
	100000,
	50000,
	20000,
	10000,
	5000
};

void game_fillRand(void *w){
	world_t (*world) = w;
	for_cells *(world->cells+i) = rand() < RAND_MAX / 5 ? 1 : 0;
	}

void game_fillMan(void *w){
	world_t (*world) = w;
	for_cells *(world->cells+i) = 0;
	gui_drawWorld(world);
	gui_edit(world);
}

void game_evolve(void *w){
	world_t (*world) = w;
	char* new;

	int i=0;
	char old[world->h][world->w];
	for_yx old[y][x] = *(world->cells + i++);


	for_y for_x{
		int n = 0;
		for (int y1 = y - 1; y1 <= y + 1; y1++)
			for (int x1 = x - 1; x1 <= x + 1; x1++)
				if (old[(y1 + world->h) % world->h][(x1 + world->w) % world->w])
					n++;

		if (old[y][x]) n--;
		*(world->cells + x + (y*world->w)) = (n == 3 || (n == 2 && old[y][x]));
	}
	world->generation++;
}

void game_save(void *w){
	world_t (*world) = w;

	char *filename;
	FILE *file;
	char aktual=0;
	unsigned pocet=-1;
	
	filename = malloc(FILENAME_MAX);

	gui_pause();
    printf("Ulozit ako: ");
    scanf("%s", filename);
    gui_resume();

	file = fopen(filename, "w");
	if(file == 0){
		perror("Chyba pri vytvarani suboru");
		return;
		}

	fprintf(file, "%u\n%u\n", world->w, world->h);
	for_cells{
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

void game_load(void *w){
	world_t (*world) = w;

	char *filename;
	FILE *file;
	char aktual=0;
	unsigned pocet, pos=0;
	
	filename = malloc(FILENAME_MAX);

	gui_pause();
    printf("Nazov suboru: ");
    scanf("%s", filename);
    gui_resume();

	file = fopen(filename, "r");
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
	free(filename);
}

void game_runner(void *w){
	world_t (*world) = w;

	char state = GAME_STATE_PAUSE;
	unsigned short rychlost = 4;
	int ch=0;

	while(toupper(ch) != 'Q') {
		ch = getch();
		if(ch>0){
			switch(toupper(ch)){
				case 'P':
				case ' ':
					if(state == GAME_STATE_RUN)
						state = GAME_STATE_PAUSE;
					else
						state = GAME_STATE_RUN;
					break;

				case 'S':
					game_save(world);
					break;

				case 'L':
					state = GAME_STATE_PAUSE;
					game_load(world);
					gui_clr();
					gui_drawWorld(world);
					break;

				case 'H':
					gui_drawGameHelp();
					break;

				case 'E':
					state = GAME_STATE_PAUSE;
					gui_edit(world);
					break;

				case '+':
				case '>':
					if(rychlost < 8)
						rychlost++;
					break;

				case '-':
				case '<':
					if(rychlost > 0)
						rychlost--;
					break;

				case '0':
					state = GAME_STATE_STEP;
					break;

				default:
					if(ch>='1' && ch<='9')
						rychlost = ch - '1';
					break;
			}
		}
		if(state == GAME_STATE_RUN || state == GAME_STATE_STEP) {
			gui_drawWorld(world);
			game_evolve(world);
		}
		if(state == GAME_STATE_STEP)
			state = GAME_STATE_PAUSE;
		else
			usleep(dly_tab[rychlost]);
	}

}
