#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "defs.h"
#include "gui.h"

void gui_init(){
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	nodelay(stdscr, true);
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_RED);
	attron(COLOR_PAIR(1));
	gui_clr();
	refresh();
}

void gui_end(){
	endwin();
	attroff(COLOR_PAIR(1));
}

void gui_pause(){
	def_prog_mode();
	endwin();
}

void gui_resume(){
	reset_prog_mode();
	refresh();
}

void gui_edit(void *w){
	world_t (*world) = w;
	int ch=0, x=0, y=0;
	move(0,0);
	curs_set(1);

	while((ch != 'E') && (ch != 'e')){
		ch = getch();
		switch(ch){
			case ' ':
				if(*(world->cells+x+(y*world->w))){
					*(world->cells+x+(y*world->w)) = 0;
					addch(' ');
					addch(' ');
				}else{
					*(world->cells+x+(y*world->w)) = 1;
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
					y=world->h-1;
					move(y,2*x);
					}
				break;
			case KEY_DOWN:
				if(y<world->h-1)
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
					x=world->w-1;
					move(y,2*x);
					}
				break;
			case KEY_RIGHT:
				if(x<world->w-1)
					move(y,2*(++x));
				else{
					x=0;
					move(y,2*x);
					}
				break;
		}
	usleep(10000);
	}
	curs_set(0);
}

void gui_clr(){
	erase();
}

void gui_showWorld(void *w){
	world_t (*world) = w;
	
	for(int y=0; y<world->h; y++){
		move(y, 0);
		for(int x=0; x<world->w; x++)
			if(*(world->cells+x+(y*world->w))){
				addch('{');
				addch('}');
			}else{
				addch(' ');
				addch(' ');
			}
	}
	refresh();
}
