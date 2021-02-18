#include "util.h"

void bubble_sort(void **niz, size_t duz, size_t (*f)(void*)) {
	if (duz < 2)
		return;
	// printf("d: %lu\n", duz);
	size_t i;
	for (i = 0; i < duz; i++) {
		int swap = 0;
		for (size_t j = 0; j < duz-1; j++) {
			if (f(niz[j]) >= f(niz[j+1])) {
				swap = 1;
				void *t = niz[j+1];
				niz[j+1] = niz[j];
				niz[j] = t;
			}
		}
		if(!swap) {
			// printf("%d\n", i);
			return;
		}
	}
	// printf("%d\n", i);
}

/* TODO: da li ostaviti ovo
static void progress_bar2(const char *text, float progress, size_t width) {
	int pr = 0;
	putchar('\r');
	if ((pr = printf("%s% 4.f%% [", text, progress)) < 0)
		return;
	pr = width - pr-1;
	for (int i = 0; i < pr; i++)
		if (100.*i/pr < progress)
			putchar('=');
		else
			putchar(' ');
	putchar(']');
}
*/

void progress_bar(float progress, size_t width, const char *fmt, ...) {
	int pr = 0, t = 0;
	putchar('\r');
	va_list ap;
	va_start(ap, fmt);
	if ((pr = vprintf(fmt, ap)) < 0) {
		va_end(ap);
		return;
	}
	if ((t = printf(" [")) < 0) {
		va_end(ap);
		return;
	}
	pr += t;
	pr = width - pr-1;
	for (int i = 0; i < pr; i++)
		if (100.*i/pr < progress)
			putchar('=');
		else
			putchar(' ');
	putchar(']');
	va_end(ap);
}

struct term_size get_term_size() {
	struct term_size t = {0};
	char *t1 = NULL;
	if ((t1 = getenv("COLUMNS"))) {
		t.w = strtol(t1, NULL, 10);
		if ((t1 = getenv("LINES"))) 
			t.h = strtol(t1, NULL, 10);

	} else {
		FILE *pipe = popen("stty size", "r");
		if (!pipe)
			return t;

		if (fscanf(pipe, "%lu %lu", &t.h, &t.w) != 2) {
			fclose(pipe);
			return (struct term_size){0, 0};
		}
		fclose(pipe);
	}
	return t;
}

double map(double x, double xmin, double xmax, double min, double max) {
	return (x - xmin)*(max - min)/(xmax - xmin) + min;
}


void putoctal(char *dst, unsigned long num, int size) {
    if (snprintf(dst, size, "%.*lo", size - 1, num) >= size)
        fprintf(stderr, "snprintf: input number too large\n");
}

