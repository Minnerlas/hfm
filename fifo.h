#ifndef FIFO_H
#define FIFO_H

#include <stdlib.h>

struct red {
	size_t stv_vel;
	size_t vel;
	size_t poc;
	size_t kraj;
	void **niz;
};

struct red *napravi_red(size_t vel);
int    dodaj_red(struct red *r, void *p);
void*  pop_red(struct red *r);

#endif /* FIFO_H */
