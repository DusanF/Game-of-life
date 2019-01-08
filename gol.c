#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include "defs.h"
#include "game.h"
#include "gui.h"


int main(int argc, char *argv[]){
	srand(time(NULL));
	system("clear");
	world_t world;
	world.w = 0;
	world.h = 0;
	world.generation = 0;

	char vstup[5];

	printf("Vitaj v Game Of Life!\nMaximalne rozmery sveta su: %dx%d\nZadaj sirku [%d] : ", MAX_W, MAX_H, DEF_W);
	if(fgets(vstup, 5, stdin) == NULL)
		world.w = DEF_W;
	else{
		world.w = atoi(vstup);
		if (world.w < MIN_W || world.h > MAX_W)
			world.w = DEF_W;
	}

	printf("Zadaj vysku [%d] : ", DEF_H);

	if(fgets(vstup, 5, stdin) == NULL)
		world.h = DEF_H;
	else{
		world.h = atoi(vstup);
		if (world.h < MIN_H || world.h > MAX_H)
			world.h = DEF_H;
	}

	world.cells = malloc(world.h * world.w * sizeof(char));	//Rozmery su uz zname, svet sa moze alokovat

	printf("Generovat nahodne? [Y/n]: ");
	if(fgets(vstup, 5, stdin) == NULL){
		gui_init();
		game_fillRand(&world);
	}else{
		gui_init();
		if (vstup[0] == 'n' || vstup[0] == 'N')
			game_fillMan(&world);
		else
			game_fillRand(&world);
	}

	gui_drawWorld(&world);									//Vygenerovany svet sa vykresli

	game_runner(&world);									//Spustenie simulacie

	gui_end();
	system("clear");
	free(world.cells);
	return 0;
}
