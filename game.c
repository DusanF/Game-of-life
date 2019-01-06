#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "defs.h"
#include "gui.h"
#include "net.h"
#include "file.h"
#include "history.h"

const unsigned dly_tab[] = {								//dostupne rychlosti simulacie (dlzka jedneho kroku v us)
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

void game_fillRand(void *w){								//Nahode naplnenie sveta
	world_t (*world) = w;

	for(int i=0; i<(world->w * world->h); i++)
		*(world->cells+i) = rand() < RAND_MAX / 5 ? 1 : 0;	//20% sanca zivota v novej bunke (1/5)
}

void game_fillMan(void *w){									//Rucne naplnenie sveta
	world_t (*world) = w;

	for(int i=0; i<(world->w * world->h); i++)
		*(world->cells+i) = 0;								//vytvor prazdny svet, potom spusti editor
	gui_drawWorld(world);
	gui_edit(world);
}

void game_evolve(void *w){									//Vypocet novej generacie
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

void game_save(void *w){									//Ukladanie aktualneho stavu do suboru / na server
	gui_pause();

	char vstup[5];
	system("clear");
	printf("Ulozit Lokalne alebo na Server? [L/s]: ");
	if(fgets(vstup, 5, stdin) != NULL && (vstup[0] == 'S' || vstup[0] == 's')){
		net_save(w);
		sleep(2);
	}
	else
		file_save(w);
	gui_resume();
}

void game_load(void *w){									//Nacitanie sveta zo suboru / zo servera
	gui_pause();

	char vstup[5];
	system("clear");
	printf("Nacitat z Lokalneho suboru alebo zo Servera? [L/s]: ");
	if(fgets(vstup, 5, stdin) != NULL && (vstup[0] == 'S' || vstup[0] == 's'))
		net_load(w);
	else
		file_load(w);

	sleep(2);
	gui_resume();
}

void game_runner(void *w){									//Hlavna funkcia, zabezpecuje obsluhu klavesnice a zobrazovanie stavu
	world_t (*world) = w;
	hist_t history;

	unsigned short rychlost = 4;
	int ch=0;

	world->state = GAME_STATE_PAUSE;
	hist_init(&history);

	gui_drawStat(world);

	while(toupper(ch) != 'Q') {
		ch = getch();
		if(ch>0){
			switch(toupper(ch)){
				case 'P':									//Pozastavenie / pokracovanie
				case ' ':
					if(world->state == GAME_STATE_RUN){
						world->state = GAME_STATE_PAUSE;
						gui_drawStat(world);
					}else
						world->state = GAME_STATE_RUN;
					break;

				case 'S':									//Ulozenie
					game_save(world);
					break;

				case 'L':									//Nacitanie
					world->state = GAME_STATE_PAUSE;
					game_load(world);
					gui_clr();
					gui_drawWorld(world);
					gui_drawStat(world);
					hist_clear(&history);
					break;

				case 'H':									//Zobrazenie helpu
					gui_drawGameHelp();
					break;

				case 'E':									//Spustenie editora
					gui_edit(world);
					world->state = GAME_STATE_PAUSE;
					gui_drawStat(world);
					break;

				case 263:									//Krok spat (backspace)
					hist_pop(&history, world);
					world->state = GAME_STATE_PAUSE;
					gui_clr();
					gui_drawWorld(world);
					gui_drawStat(world);
					break;

				case '+':									//Zrychlenie
				case '>':
					if(rychlost < 8)
						rychlost++;
					world->state = GAME_STATE_RUN;
					break;

				case '-':									//Spomalenie
				case '<':
					if(rychlost > 0)
						rychlost--;
					world->state = GAME_STATE_RUN;
					break;

				case '0':									//Jeden krok
					world->state = GAME_STATE_STEP;
					break;

				default:
					if(ch>='1' && ch<='9'){					//Zvolenie rychlosti
						rychlost = ch - '1';
						world->state = GAME_STATE_RUN;
					}
					break;
			}
		}
		if(world->state == GAME_STATE_RUN || world->state == GAME_STATE_STEP) {	//Nova generacia iba ak je simulacia spustena / jeden krok
			hist_push(&history, world);						//Aktualna generacia sa ulozi do historie, az potom sa vygeneruje nova
			game_evolve(world);
			gui_drawWorld(world);
			gui_drawStat(world);
		}
		if(world->state == GAME_STATE_STEP){				//Po vykonani jedneho kroku simulaciu pozastav
			world->state = GAME_STATE_PAUSE;
			gui_drawStat(world);
		}else
			usleep(dly_tab[rychlost]);						//Spomalenie zobrazovania podla zvolenej rychlosti
	}
	hist_clear(&history);									//Po skonceni je potrebne vycistit historiu (dealokovat)

}
