#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

#include "hfm.h"
#include "fifo.h"
#include "util.h"

#define BARSPLIT 33.

char help_txt[] = 
"HFM file compression v0.0.1alpha\n"
" -o	Output file\n"
" -x	Extract file\n"
" -h	Display this help and exit\n";

size_t bytes[MAX_U8] = {0};

struct par parovi [MAX_U8] = {0};
struct par *pbytes[MAX_U8] = {0};

struct clan clanovi[MAX_U8] = {0};
struct clan bajtovi[MAX_U8] = {0};
struct clan *red_bajtova[MAX_U8] = {0};

struct kod kodovi[MAX_U8] = {0};
struct hfm_header h = {0};

size_t f_char(void *p) {
	return ((struct par*)p)->tezina;
}

size_t f_clan(void *p) {
	return ((struct clan*)p)->tezina;
}

int main(int argc, char **argv) {
	char *ulaz = NULL;
	char *izlaz = "izlaz.hfm";
	// char *izlaz = NULL;
	int verbose = 0;
	int extract = 0;
	if (argc == 1) {
		printf(help_txt);
		exit(0);
	}
	for (int i = 1; i < argc; i++) {
		// printf("%s\n", argv[i]);
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'o':
					izlaz = argv[++i];
					break;

				case 'v':
					verbose = 1;
					break;

				case 'x':
					extract = 1;
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

	int ansi_term = 0;
	size_t term_width = 0;
	{

		char *t = getenv("TERM");
		if (t && *t && strcmp(t, "dumb"))
			ansi_term = 1;
		if (ansi_term)
			term_width = get_term_size().w;
	}
	// printf("TERM: %s\t%lu\n", ansi_term ? "ansi" : "dumb", term_width);

	if (!extract) {
		/* COMPRESS */
		if (!ulaz)
			fprintf(stderr, "No input file\n"), exit(1);

		printf("ulaz:  %s\n", ulaz);
		printf("izlaz: %s\n", izlaz);

		size_t br_bajtova = 0;

		FILE *ul = fopen(ulaz, "r");
		if (!ul)
			perror("Can't open input file"), exit(1);

		if (fseek(ul, 0L, SEEK_END))
			perror("Can't determine the size of the file"), exit(1);
		ssize_t f_vel = 0;
		if ((f_vel = ftell(ul)) < 0)
			/* Should never happen */
			perror("Can't seek to the start of the file"), exit(1);
		rewind(ul);

		FILE *iz = NULL;
		if (izlaz)
			iz = fopen(izlaz, "w");
		else
			iz = stdout;

		if (!iz)
			perror("Can't open output file"), fclose(ul), exit(1);

		/* CITANJE */
		{
			uint8_t c;
			int progress = 0;
			float t;
			if (ansi_term)
				printf("\x1b[?25l");
			while (fread(&c, 1, 1, ul) > 0) {
				// printf("%d\n", (int)c);
				br_bajtova++;
				bytes[c]++;
				// t = map((double)br_bajtova/f_vel, 0, 1, 0, 100);
				t = br_bajtova*1./f_vel;
				if ((f_vel > (1<<17)) 
						&& (progress - (int)(map(t, 0, 1, 0, BARSPLIT)))) {
					// && (progress - (int)(br_bajtova*100./f_vel))) {
					// progress = br_bajtova*100./f_vel;
					progress = map(t, 0, 1, 0, BARSPLIT);
					if (ansi_term) {
						progress_bar(progress, term_width, 
								"Progress: % 3d%%", progress);

						fflush(stdout);
					} else {
						if (progress % 10 == 0)
							printf("Progress: % 3d%%\n", progress);
					}
				}
				// sleep(1);
			}
			if (ansi_term)
				printf("\x1b[?25h");
		}


		/*
		   for (int i = 0; i < MAX_U8; i++)
		   printf("%3d: %5lu\n", i, bytes[i]);
		   */

		for (int i = 0; i < MAX_U8; i++) {
			parovi[i] = (struct par){i, bytes[i]};
			pbytes[i] = &parovi[i];
		}

		bubble_sort((void**)pbytes, MAX_U8, f_char);

		/*
		   for (int i = 0; i < 256; i++)
		   printf("i=%d: b = %d , w = %lu\n",
		   i, pbytes[i]->bajt, pbytes[i]->tezina);
		   */

		for (int i = 0; i < MAX_U8; i++) {
			clanovi[i] = (struct clan){
				.tezina = pbytes[i]->tezina,
				.tip    = LIST,
				.u.bajt = pbytes[i]->bajt,
			};
			red_bajtova[i] = &clanovi[i];
		}

		int poc = 0, kraj = MAX_U8;
		int aloc = 0;

		while (poc < kraj-1) {
			struct clan *t1 = red_bajtova[poc], *t2 = red_bajtova[poc+1];
			struct clan *t3 = &bajtovi[aloc++];
			*t3 = (struct clan){
				.tezina = t1->tezina + t2->tezina,
				.tip    = CVOR,
				.u.p.l  = t1,
				.u.p.d  = t2,
			};
			red_bajtova[++poc] = t3;
			bubble_sort((void**)(red_bajtova+poc), kraj-poc, f_clan);
		}

		/*
		   printf("s:  %lu\n", f_vel);
		   printf("u:  %lu\n", br_bajtova);
		   printf("tu: %lu\n", red_bajtova[kraj-1]->tezina);
		   */

		stablo_u_tabelu(&kodovi[0], red_bajtova[kraj-1]);

		/* PISANJE */
		size_t napisano = 0;
		{
			size_t procitano = 0;
			int progress = 0;
			uint8_t c;
			uint8_t buf = 0;
			int buf_vel = 0;
			struct stat st;
			stat(ulaz, &st);
			rewind(ul);
			printf("\x1b[?25l");
			/* HEADER */
			h.magic[0] = HFM_MAGIC0;
			h.magic[1] = HFM_MAGIC1;
			h.magic[2] = HFM_MAGIC2;
			h.magic[3] = HFM_MAGIC3;
			h.ver[0] = h.ver[1] = 0;
			putoctal(h.size,  f_vel,                     sizeof(h.size));
			putoctal(h.mtime, (size_t)st.st_mtime,       sizeof(h.mtime));
			putoctal(h.uid,   (size_t)st.st_uid,         sizeof(h.uid));
			putoctal(h.gid,   (size_t)st.st_gid,         sizeof(h.gid));
			putoctal(h.mode,  (size_t)st.st_mode & 0777, sizeof(h.gid));
			strncpy(h.name,   basename(ulaz),            sizeof(h.name));
			for (int i = 0; i < MAX_U8; i++)
				h.weights[i] = bytes[i];

			if(!fwrite(&h, sizeof(h), 1, iz))
				perror("Error while writing to output file"), 
					fclose(ul), fclose(iz), exit(1);
			/* CONTENTS */
			while (fread(&c, 1, 1, ul) > 0) {
				for (unsigned i = 0; i < kodovi[c].br; i++) {
					if (buf_vel == 8) {
						if (!fwrite(&buf, 1, 1, iz))
							perror("Error while writing to output file"), 
								fclose(ul), fclose(iz), exit(1);

						buf_vel = 0;
						napisano++;
					}

					buf_vel++;
					buf = (buf << 1) | ((kodovi[c].bitovi >> i) & 1 );
				}
				procitano++;
				float t = procitano*1./f_vel;
				if ((f_vel > (1<<17)) 
					   && (progress - (int)(map(t, 0, 1, BARSPLIT, 100)))) {
					progress = map(t, 0, 1, BARSPLIT, 100);
					if (ansi_term) {
						progress_bar(progress, term_width, 
								"Progress: % 3d%%", progress);

						fflush(stdout);
					} else {
						if (progress % 10 == 0)
							printf("Progress: % 3d%%\n", progress);
					}
				}
			}
			if (buf_vel) {
				buf <<= 8 - buf_vel;
				fwrite(&buf, 1, 1, iz);
				napisano++;
			}
			printf("\x1b[?25h");
		}

		/* VERBOSE PRINTING */
		size_t ukup = 0;
		for (int i = 0; i < MAX_U8; i++) {
			ukup += kodovi[i].br * parovi[i].tezina;

			if (verbose) {
				printf("%3d %3d ", i, kodovi[i].br);
				uint64_t t = kodovi[i].bitovi;
				for (int j = 0; j < kodovi[i].br; j++) {
					if (t & 1)
						printf("1");
					else
						printf("0");
					t >>= 1;
				}
				putchar('\n');
			}
		}

		printf("\ns:   %lu\n", f_vel);
		printf("u:   %lu\n", br_bajtova);
		printf("tu:  %lu\n", red_bajtova[kraj-1]->tezina);
		if (verbose) {
			printf("u:   %lu\n", br_bajtova);
			printf("tu:  %lu\n", red_bajtova[kraj-1]->tezina);
		}

		printf("ouk: %.3f\n", sizeof(h)+ukup/8.);
		printf("ocr: %.3f%%\n", 100*(sizeof(h)+ukup/8.)/br_bajtova);

		printf("uk:  %lu\n", sizeof(h)+napisano);
		printf("cr:  %.3f%%\n", 100.*(sizeof(h)+napisano)/br_bajtova);

		if (iz != stdout) {
			fclose(iz);
			iz = NULL;
		}
		fclose(ul);
		ul = NULL;
	} else {
		/* EXTRACT */
		if (!ulaz)
			fprintf(stderr, "No input file\n"), exit(1);

		printf("ulaz:  %s\n", ulaz);
		FILE *ul = fopen(ulaz, "r");
		if (!ul)
			perror("Can't open input file"), exit(1);

		int t = 0;
		if ((t = fread(&h, sizeof(h), 1, ul) < 1) 
				|| !hfm_check_header(&h)) {
			if (!hfm_check_header(&h))
				fprintf(stderr, "Invalid file header\n");
			else if (t < 0)
				perror("Error while reading from input file");
			else if (t < 1)
				fprintf(stderr, "File too small\n");

			fclose(ul);
			exit(1);
		}

		size_t isize = 0;
		{
			char buff[sizeof(h.size)+1] = {0};
			strncpy(buff, h.size, sizeof(buff)-1);
			sscanf(buff, "%lo", &isize);
		}

		long mode, mtime, uid, gid;
		char *p;
		if ((mode  = strtol(h.mode,  &p, 8)) < 0 || *p != '\0')
			fprintf(stderr, "Invalid number\n"), fclose(ul), exit(1);
		if ((mtime  = strtol(h.mtime,  &p, 8)) < 0 || *p != '\0')
			fprintf(stderr, "Invalid number\n"), fclose(ul), exit(1);
		if ((uid  = strtol(h.uid,  &p, 8)) < 0 || *p != '\0')
			fprintf(stderr, "Invalid number\n"), fclose(ul), exit(1);
		if ((gid  = strtol(h.gid,  &p, 8)) < 0 || *p != '\0')
			fprintf(stderr, "Invalid number\n"), fclose(ul), exit(1);

		printf("izlaz: %s\n", h.name);
		printf("s: %lu\n", isize);

		FILE *iz = fopen(h.name, "w");
		if (!iz)
			perror("Couldn't open output file"), fclose(ul), exit(1);


		/* PRAVLJENJE STABLA */
		for (int i = 0; i < MAX_U8; i++) {
			parovi[i] = (struct par){i, h.weights[i]};
			pbytes[i] = &parovi[i];
		}

		bubble_sort((void**)pbytes, MAX_U8, f_char);

		/*
		   for (int i = 0; i < 256; i++)
		   printf("i=%d: b = %d , w = %lu\n",
		   i, pbytes[i]->bajt, pbytes[i]->tezina);
		   */

		for (int i = 0; i < MAX_U8; i++) {
			clanovi[i] = (struct clan){
				.tezina = pbytes[i]->tezina,
				.tip    = LIST,
				.u.bajt = pbytes[i]->bajt,
			};
			red_bajtova[i] = &clanovi[i];
		}

		int poc = 0, kraj = MAX_U8;
		int aloc = 0;

		while (poc < kraj-1) {
			struct clan *t1 = red_bajtova[poc], *t2 = red_bajtova[poc+1];
			struct clan *t3 = &bajtovi[aloc++];
			*t3 = (struct clan){
				.tezina = t1->tezina + t2->tezina,
				.tip    = CVOR,
				.u.p.l  = t1,
				.u.p.d  = t2,
			};
			red_bajtova[++poc] = t3;
			bubble_sort((void**)(red_bajtova+poc), kraj-poc, f_clan);
		}

		/*
		   printf("s:  %lu\n", f_vel);
		   printf("u:  %lu\n", br_bajtova);
		   printf("tu: %lu\n", red_bajtova[kraj-1]->tezina);
		   */

		stablo_u_tabelu(&kodovi[0], red_bajtova[kraj-1]);
		struct clan *koren = red_bajtova[kraj-1];

		/* VERBOSE PRINTING */
		// size_t ukup = 0;
		for (int i = 0; i < MAX_U8; i++) {
			// ukup += kodovi[i].br * parovi[i].tezina;

			if (verbose) {
				printf("%3d %3d ", i, kodovi[i].br);
				uint64_t t = kodovi[i].bitovi;
				for (int j = 0; j < kodovi[i].br; j++) {
					if (t & 1)
						printf("1");
					else
						printf("0");
					t >>= 1;
				}
				putchar('\n');
			}
		}

		/* DESIFROVANJE FAJLA */
		{
			size_t napisano = 0;
			int buf_size = 0;
			uint8_t buf = 0;
			int progress = 0;
			struct clan *t = koren;
			if (ansi_term)
				printf("\x1b[?25l");
			while (napisano < isize) {
				if (!buf_size) {
					if (!fread(&buf, 1, 1, ul))
						perror("Couldn't read from input file"), 
							fclose(ul), fclose(iz), exit(1);
					buf_size = 8;
				}
				if (buf & (1 << (--buf_size)))
					t = t->u.p.d;
				else
					t = t->u.p.l;

				if (t->tip == LIST) {
					if (!fwrite(&t->u.bajt, 1, 1, iz))
						perror("Couldn't write to the output file"), 
							fclose(ul), fclose(iz), exit(1);

					napisano++;
					t = koren;
				}

				float t = napisano*1./isize;
				if ((isize > (1<<17)) 
					   && (progress - (int)(map(t, 0, 1, 0, 100)))) {
					progress = map(t, 0, 1, 0, 100);
					if (ansi_term) {
						progress_bar(progress, term_width, 
								"Progress: % 3d%%", progress);

						fflush(stdout);
					} else {
						if (progress % 10 == 0)
							printf("Progress: % 3d%%\n", progress);
					}
				}
			}
			if (ansi_term)
				printf("\x1b[?25h");
		}

		fclose(iz);

		if (!getuid() && chown(h.name, uid, gid))
			perror(NULL), fclose(ul), exit(1);
		if (chmod(h.name, mode) < 0)
			fprintf(stderr, "fchmod %s:", h.name), perror(NULL);

		fclose(ul);
	}
}
