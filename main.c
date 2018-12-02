#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include "defs.h"
#include "game.h"
#include "gui.h"


int main(int argc, char *argv[]){
	srand(time(NULL));
	system("clear");
	int w = 0, h = 0;

	printf("Víta ťa Game Of Life (GoL)!\n\nZadaj pocet riadkov: ");
	scanf("%d", &h);
	if (h < 5 || h > 65)
		h = 30;

	printf("Zadaj pocet stlpcov : ");
	scanf("%d", &w);
	if (w < 5 || w > 135)
		w = 50;

	char vstup = 0;
	while ((vstup = getchar()) != '\n' && vstup != EOF);

	char univ[h][w];

	printf("Generovat nahodne? [Y/n]: ");
	scanf("%c", &vstup);

	gui_init();

	if (vstup == 'n' || vstup == 'N')
		game_fillMan(univ, w, h);
	else
		game_fillRand(univ, w, h);


	gui_showUniv(univ, w, h);

	hra(univ, w, h);

	gui_end();
	system("clear");
	return 0;
}
