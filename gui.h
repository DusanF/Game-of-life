#ifndef _GUI_H
#define _GUI_H

void gui_init();
void gui_end();
void gui_pause();
void gui_resume();
void gui_clr();
void gui_drawWorld(world_t *);
void gui_drawStat(world_t *);
void gui_edit(world_t *);
void gui_drawGameHelp();
void gui_drawEditHelp();

#endif
