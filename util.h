#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>

struct term_size {
	size_t w;
	size_t h;
};

void bubble_sort(void **niz, size_t duz, size_t (*f)(void*));
void progress_bar(float progress, size_t width, const char *fmt, ...);
struct term_size get_term_size();
double map(double x, double xmin, double xmax, double min, double max);
void putoctal(char *dst, unsigned long num, int size);

#endif /* UTIL_H */
