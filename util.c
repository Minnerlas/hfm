#include "util.h"

void bubble_sort(void **niz, size_t duz, int (*f)(void*)) {
	for (size_t i = 0; i < duz; i++) {
		int swap = 0;
		for (size_t j = 0; j < duz-1; j++) {
			if (f(niz[j]) >= f(niz[j+1])) {
				swap = 1;
				void *t = niz[j+1];
				niz[j+1] = niz[j];
				niz[j] = t;
			}
		}
		if(!swap)
			return;
	}

}
