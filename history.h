#ifndef _HIST_H
#define _HIST_H

unsigned hist_push(hist_t *, world_t *);
unsigned hist_pop(hist_t *, world_t *);
void hist_clear(hist_t *);
void hist_init(hist_t *);


#endif
