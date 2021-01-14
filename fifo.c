#include "fifo.h"

struct red *napravi_red(size_t vel) {
	struct red *t = malloc(sizeof(*t));
	if (!t)
		return NULL;
	void **niz = malloc(sizeof(*niz)*vel);
	if (!niz) {
		free(t);
		return NULL;
	}
	*t = (struct red){vel, 0, 0, 0, niz};
	return t;
}

int dodaj_red(struct red *r, void *p) {
	if (r->stv_vel == r->vel)
		return 0;
	r->niz[r->kraj] = p;
	r->kraj = (r->kraj + 1) % r->stv_vel;
	r->vel++;
	return 1;
}

void* pop_red(struct red *r) {
	if (r->vel == 0)
		return NULL;
	void *t = r->niz[r->poc];
	r->poc = (r->poc + 1) % r->stv_vel;
	r->vel--;
	return t;
}
