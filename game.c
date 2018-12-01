#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
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

void naplnNahodne(void *u, int w, int h){
    char (*univ)[w] = u;
    for_xy univ[y][x] = rand() < RAND_MAX / 5 ? 1 : 0;
	}

void evolve(void *u, int w, int h){
    char (*univ)[w] = u;
    char new[h][w];

    for_y for_x{
        int n = 0;
        for (int y1 = y - 1; y1 <= y + 1; y1++)
            for (int x1 = x - 1; x1 <= x + 1; x1++)
                if (univ[(y1 + h) % h][(x1 + w) % w])
                    n++;

        if (univ[y][x]) n--;
        new[y][x] = (n == 3 || (n == 2 && univ[y][x]));}
    for_y for_x univ[y][x] = new[y][x];
}

void saveUniv(void *u, int w, int h){
    char (*univ)[w] = u;
    
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

	fprintf(file, "%d\n%d\n", w, h);
	for_yx {
		pocet++;
		if(univ[y][x] != aktual){
			fprintf(file, "%d\n", pocet);
			pocet = 0;
			aktual = univ[y][x];
		}
			
	}
	fclose(file);
}

void hra(void *u, int w, int h, void *klav_str){
    char (*univ)[w] = u;
	KLAVESA *kla = (KLAVESA*) klav_str;

    int stav = STAV_PAUSE;
    unsigned short rychlost = 3;

    while (1) {
		pthread_mutex_lock(&(kla->kl_mut));
        if (kla->zmena) {
            kla->zmena = 0;
            switch(toupper(kla->tlacidlo)){
				case 'P':
				case ' ':
					if(stav == STAV_RUN)
						stav = STAV_PAUSE;
					else
						stav = STAV_RUN;
					break;
				
				case 'S':
					saveUniv(univ, w, h);
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
					stav = STAV_STEP;
					break;
					
				default:
					if(kla->tlacidlo>='1' && kla->tlacidlo<='9')
						rychlost = kla->tlacidlo - '1';
					break;
			}
        }
        pthread_mutex_unlock(&(kla->kl_mut));
        if(stav == STAV_RUN || stav == STAV_STEP) {
            gui_showUniv(univ, w, h);
            evolve(univ, w, h);
        }
        if(stav == STAV_STEP)
			stav = STAV_PAUSE;
		else
			usleep(dly_tab[rychlost]);
    }

}
