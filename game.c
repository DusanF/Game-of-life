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
#include "generator.h"


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

void game_fillRand(world_t *world){								//Nahode naplnenie sveta
	for(int i=0; i<(world->w * world->h); i++)
		*(world->cells+i) = rand() < RAND_MAX / 5 ? 1 : 0;	//20% sanca zivota v novej bunke (1/5)
}

void game_fillMan(world_t *world){									//Rucne naplnenie sveta
	for(int i=0; i<(world->w * world->h); i++)
		*(world->cells+i) = 0;								//vytvor prazdny svet, potom spusti editor
	gui_drawWorld(world);
	gui_edit(world);
}

void game_save(world_t *world){									//Ukladanie aktualneho stavu do suboru / na server
	gui_pause();

	char vstup[5];
	system("clear");
	printf("Ulozit Lokalne alebo na Server? [L/s]: ");
	if(fgets(vstup, 5, stdin) != NULL && (vstup[0] == 'S' || vstup[0] == 's')){
		net_save(world);
		sleep(2);
	}
	else
		file_save(world);
	gui_resume();
}

void game_load(world_t *world){									//Nacitanie sveta zo suboru / zo servera
	gui_pause();

	char vstup[5];
	system("clear");
	printf("Nacitat z Lokalneho suboru alebo zo Servera? [L/s]: ");
	if(fgets(vstup, 5, stdin) != NULL && (vstup[0] == 'S' || vstup[0] == 's'))
		net_load(world);
	else
		file_load(world);

	sleep(2);
	gui_resume();
}

void game_runner(world_t *world){									//Hlavna funkcia, zabezpecuje obsluhu klavesnice a zobrazovanie stavu
	hist_t history;

	gen_struct_t genstr;

	unsigned short rychlost = 4;
	int ch=0;

	world->state = GAME_STATE_PAUSE;
	hist_init(&history);

	gui_drawStat(world);
	gen_init(&genstr);
	gen_loadWorld(&genstr, world);

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
					world->generation = 0;
					gui_drawWorld(world);
					gui_drawStat(world);
					gen_loadWorld(&genstr, world);
					hist_clear(&history);
					break;

				case 'H':									//Zobrazenie helpu
					gui_drawGameHelp();
					break;

				case 'E':									//Spustenie editora
					gui_edit(world);
					world->generation = 0;
					gen_loadWorld(&genstr, world);
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
			gen_read(&genstr, world);

			gui_drawWorld(world);
			gui_drawStat(world);
		}
		if(world->state == GAME_STATE_STEP){				//Po vykonani jedneho kroku simulaciu pozastav
			world->state = GAME_STATE_PAUSE;
			gui_drawStat(world);
		}else
			usleep(dly_tab[rychlost]);						//Spomalenie zobrazovania podla zvolenej rychlosti
	}
	hist_clear(&history);									//Upratanie po skonceni
	gen_clear(&genstr);

}
