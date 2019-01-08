#ifndef _GEN_H
#define _GEN_H

void gen_init(gen_struct_t *);
void gen_clear(gen_struct_t *);
void gen_loadWorld(gen_struct_t *, world_t *);
void gen_read(gen_struct_t *, world_t *);
void *generator(void *);

#endif
