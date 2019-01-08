#ifndef _GAME_H
#define _GAME_H

void game_fillRand(world_t *);
void game_fillMan(world_t *);
void game_evolve(world_t *);
void game_save(world_t *);
void game_load(world_t *);
void game_runner(world_t *);


#endif
