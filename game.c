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
	gui_showUniv(world);
	gui_edit(world);
}

void game_evolve(void *w){
	world_t (*world) = w;
	char* new;
	//new = malloc((world->w)*(world->h));
	
	//free(new);
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
}

void game_save(void *w){
	world_t (*world) = w;

	char *filename = "save.gol";
	FILE *file;
	char aktual=0;
	unsigned pocet=-1;

//    printf("Save as: ");
//    scanf("%s", filename);

	file = fopen(filename, "w");
	if(file == 0){
		perror("Chyba pri vytvarani suboru");
		return;
		}

	fprintf(file, "%d\n%d\n", world->w, world->h);
	for_cells{
		pocet++;
		if(*world->cells != aktual){
			fprintf(file, "%d\n", pocet);
			pocet = 0;
			aktual = *world->cells;
		}
	}
	fclose(file);
}

void hra(void *w){
	world_t (*world) = w;

	int stav = GAME_STATE_PAUSE;
	unsigned short rychlost = 3;
	int ch=0;

	while (1) {
		ch = getch();
		if(ch>0){
			switch(toupper(ch)){
				case 'P':
				case ' ':
					if(stav == GAME_STATE_RUN)
						stav = GAME_STATE_PAUSE;
					else
						stav = GAME_STATE_RUN;
					break;

				case 'S':
					game_save(world);
					break;
				case 'E':
					stav = GAME_STATE_PAUSE;
					gui_edit(world);
					break;

				case 'Q':
					return;

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
					stav = GAME_STATE_STEP;
					break;

				default:
					if(ch>='1' && ch<='9')
						rychlost = ch - '1';
					break;
			}
		}
		if(stav == GAME_STATE_RUN || stav == GAME_STATE_STEP) {
			gui_showUniv(world);
			game_evolve(world);
		}
		if(stav == GAME_STATE_STEP)
			stav = GAME_STATE_PAUSE;
		else
			usleep(dly_tab[rychlost]);
	}

}
