#ifndef HFM_H
#define HFM_H

#define HFM_MAGIC0    0x48
#define HFM_MAGIC1    0x46
#define HFM_MAGIC2    0x4d
#define HFM_MAGIC3    0x00

#define MAX_U8 256

#include <stdint.h>
#include <stddef.h>

struct par {
	uint8_t bajt;
	size_t tezina;
};

struct clan {
	size_t tezina;
	enum {
		CVOR,
		LIST,
	} tip;

	union {
		struct {
			struct clan *l;
			struct clan *d;
		} p;
		uint8_t bajt;
	} u;
};

struct kod {
	uint8_t br;
	uint64_t bitovi;
};

struct hfm_header {
	uint8_t magic[4];
    char name[256];
    char size[12];
    char mtime[12];
    char mode[8];
    char uid[8];
    char gid[8];
	char ver[2];
	size_t weights[256];
};

int stablo_u_tabelu(struct kod *kodovi, struct clan *koren);
int hfm_check_header(struct hfm_header *h);

#endif /* HFM_H */
