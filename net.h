#ifndef _NET_H
#define _NET_H

void net_save(world_t *);
void net_load(world_t *);
int net_connect(int *, int, char *);
void net_close(int);
int net_rewrite(int);
int net_login(int, char *);
int net_setFileName(int, char*);
int net_sendWorld(int, world_t *);
void net_readWorld(world_t *, unsigned char *, unsigned);

#endif
