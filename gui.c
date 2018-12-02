#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <ncurses.h>
#include "defs.h"
#include "gui.h"

void gui_init(){
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	refresh();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_RED);
	attron(COLOR_PAIR(1));
}

void gui_end(){
	endwin();
	attroff(COLOR_PAIR(1));
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

void gui_edit(void *u, int w, int h){
    char (*univ)[w] = u;
    int ch=0, x=0, y=0;
    move(0,0);
    curs_set(1);

    while((ch != 'E') && (ch != 'e')){
		ch = getch();
		switch(ch){
			case ' ':
				if(univ[y][x]){
					univ[y][x] = 0;
					addch(' ');
					addch(' ');
				}else{
					univ[y][x] = 1;
					addch('{');
					addch('}');
				}
				refresh();
				move(y,2*x);
				break;
			case KEY_UP:
				if(y>0)
					move(--y,2*x);
				else{
					y=h-1;
					move(y,2*x);
					}
				break;
			case KEY_DOWN:
				if(y<h-1)
					move(++y,2*x);
				else{
					y=0;
					move(y,2*x);
					}
				break;
			case KEY_LEFT:
				if(x>0)
					move(y,2*(--x));
				else{
					x=w-1;
					move(y,2*x);
					}
				break;
			case KEY_RIGHT:
				if(x<w-1)
					move(y,2*(++x));
				else{
					x=0;
					move(y,2*x);
					}
				break;
		}
	}
	curs_set(0);
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
				//addch(' '|A_REVERSE);
				//addch(' '|A_REVERSE);
				addch('{');
				addch('}');
			}else{
				addch(' ');
				addch(' ');
			}
		}
	}
	refresh();

}
