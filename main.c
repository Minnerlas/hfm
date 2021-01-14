#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "hfm.h"
#include "fifo.h"
#include "util.h"

char help_txt[] = 
"HFM file compression\n"
"-o		Output file\n";

size_t bytes[MAX_U8] = {0};
struct par {
	uint8_t bajt;
	size_t tezina;
} parovi [MAX_U8] = {0};
struct par *pbytes[MAX_U8] = {0};

int f_char(void *p) {
	return ((struct par*)p)->tezina;
}

int main(int argc, char **argv) {
	char *ulaz = NULL;
	char *izlaz = "izlaz.hfm";
	for (int i = 1; i < argc; i++) {
		// printf("%s\n", argv[i]);
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'o':
					izlaz = argv[++i];
					break;

				default:
					printf("Unknown option: %c\n", argv[i][1]);
					/* fallthrough */
				case 'h':
					printf(help_txt);
					exit(0);
					break;
			}
		} else {
			ulaz = argv[i];
		}
	}

	if (!ulaz)
		fprintf(stderr, "No input file\n"), exit(1);

	printf("ulaz:  %s\n", ulaz);
	printf("izlaz: %s\n", izlaz);

	size_t br_bajtova = 0;
	FILE *ul = fopen(ulaz, "r");
	if (!ul)
		perror("Couldn't open input file"), exit(1);
	{
		uint8_t c;
		while (fread(&c, 1, 1, ul)) {
			// printf("%d\n", (int)c);
			br_bajtova++;
			bytes[c]++;
		}
	}

	fclose(ul);

	// for (int i = 0; i < MAX_U8; i++)
	// 	printf("%3d: %5lu\n", i, bytes[i]);

	for (int i = 0; i < MAX_U8; i++) {
		parovi[i] = (struct par){i, bytes[i]};
		pbytes[i] = &parovi[i];
	}

	bubble_sort((void**)pbytes, MAX_U8, f_char);

	for (int i = 0; i < 256; i++)
		printf("i=%d: b = %d , w = %lu\n",
				i, pbytes[i]->bajt, pbytes[i]->tezina);

}
