#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <ncurses.h>
#include "defs.h"

void gui_init(){
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	refresh();
}

void gui_end(){
	endwin();
}

void *obsluhaKlavesnice(void *klav_str){
	KLAVESA *kla = (KLAVESA*) klav_str;
    int znak;
    while (1) {
        //znak = getc(stdin);
        znak = getch();

		pthread_mutex_lock(&(kla->kl_mut));
        kla->tlacidlo = znak;
        kla->zmena = 1;
		pthread_mutex_unlock(&(kla->kl_mut));


        if (toupper(znak) == 'Q') {
            break;
        }
    }
    return NULL;
}

void naplnRucne(void *u, int w, int h){
    char (*univ)[w] = u;
    //for_xy univ[y][x] = rand() < RAND_MAX / 2 ? 1 : 0;
    for_xy univ[y][x] = 0;
    univ[0][2]=1;
    univ[1][1]=1;
    univ[1][3]=1;
    univ[2][1]=1;
    univ[2][3]=1;
    univ[3][2]=1;
}

void gui_showUniv(void *u, int w, int h){
    char (*univ)[w] = u;
    for_y{
		move(y, 0);
		for_x{
			if(univ[y][x]){
				//addch(ACS_CKBOARD);
				//addch(ACS_CKBOARD);
				//addch(ACS_BLOCK);
				//addch(ACS_BLOCK);
				addch(' '|A_REVERSE);
				addch(' '|A_REVERSE);
			}else{
				addch(' ');
				addch(' ');
			}
		}
	}
	refresh();

}
