#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "defs.h"
#include "gui.h"
#include "net.h"
#include "file.h"

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

	for(int i=0; i<(world->w * world->h); i++)
		*(world->cells+i) = rand() < RAND_MAX / 5 ? 1 : 0;	//20% sanca zivota v novej bunke (1/5)
}

void game_fillMan(void *w){
	world_t (*world) = w;
	for(int i=0; i<(world->w * world->h); i++)
		*(world->cells+i) = 0;								//vytvor prazdny svet, potom spusti editor
	gui_drawWorld(world);
	gui_edit(world);
}

void game_evolve(void *w){
	world_t (*world) = w;
	char* new;

	int i=0;
	char old[world->h][world->w];

	for(int y = 0; y < world->h; y++)
		for(int x = 0; x < world->w; x++)
			old[y][x] = *(world->cells + i++);


	for(int y = 0; y < world->h; y++)
		for(int x = 0; x < world->w; x++){
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
	gui_pause();

	char vstup;
	printf("Ulozit Lokalne alebo na Server? [L/s]: ");
	scanf("%c", &vstup);
	
	if (vstup == 'S' || vstup == 's')
		net_save(w);
	else
		file_save(w);
	gui_resume();
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

	world->state = GAME_STATE_PAUSE;
	unsigned short rychlost = 4;
	int ch=0;

	gui_drawStat(world);

	while(toupper(ch) != 'Q') {
		ch = getch();
		if(ch>0){
			switch(toupper(ch)){
				case 'P':
				case ' ':
					if(world->state == GAME_STATE_RUN){
						world->state = GAME_STATE_PAUSE;
						gui_drawStat(world);
					}else
						world->state = GAME_STATE_RUN;
					break;

				case 'S':
					game_save(world);
					break;

				case 'L':
					world->state = GAME_STATE_PAUSE;
					game_load(world);
					gui_clr();
					gui_drawWorld(world);
					gui_drawStat(world);
					break;

				case 'H':
					gui_drawGameHelp();
					break;

				case 'E':
					world->state = GAME_STATE_EDIT;
					gui_edit(world);
					world->state = GAME_STATE_PAUSE;
					gui_drawStat(world);
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
					world->state = GAME_STATE_STEP;
					break;

				default:
					if(ch>='1' && ch<='9')
						rychlost = ch - '1';
					break;
			}
		}
		if(world->state == GAME_STATE_RUN || world->state == GAME_STATE_STEP) {
			game_evolve(world);
			gui_drawWorld(world);
			gui_drawStat(world);
		}
		if(world->state == GAME_STATE_STEP){
			world->state = GAME_STATE_PAUSE;
			gui_drawStat(world);
		}else
			usleep(dly_tab[rychlost]);
	}

}
