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


	printf("Víta ťa Game Of Life (GoL)!\n\nZadaj pocet riadkov: ");
	scanf("%d", &(world.h));
	if (world.h < 5 || world.h > 65)
		world.h = 30;

	printf("Zadaj pocet stlpcov : ");
	scanf("%d", &(world.w));
	if (world.w < 5 || world.w > 135)
		world.w = 50;

	world.cells = malloc(world.h * world.w * sizeof(char));

	char vstup = 0;
	while ((vstup = getchar()) != '\n' && vstup != EOF);

	printf("Generovat nahodne? [Y/n]: ");
	scanf("%c", &vstup);

	gui_init();

	if (vstup == 'n' || vstup == 'N')
		game_fillMan(&world);
	else
		game_fillRand(&world);

	gui_drawWorld(&world);
	game_runner(&world);

	gui_end();
	system("clear");
	free(world.cells);
	return 0;
}
