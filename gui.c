#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "defs.h"

void *obsluhaKlavesnice(void *klav_str){
	KLAVESA *kla = (KLAVESA*) klav_str;
    int znak;
    system("/bin/stty raw");
    while (1) {
        znak = getc(stdin);

		pthread_mutex_lock(&(kla->kl_mut));
        kla->tlacidlo = znak;
        kla->zmena = 1;
		pthread_mutex_unlock(&(kla->kl_mut));


        if (toupper(znak) == 'Q') {
            break;
        }
    }
    system("/bin/stty cooked");
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

void show(void *u, int w, int h){
    char (*univ)[w] = u;
    printf("\033[H");
    for_y{
        for_x printf(univ[y][x] ? "\033[07m  \033[m" : "  ");
        printf("\033[E");}
    fflush(stdout);
}
