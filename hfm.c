#include "hfm.h"

static int _stablo_u_tabelu(struct kod *kod, 
		struct clan *kor, uint64_t bin, int n) {
	if(kor->tip == CVOR) {
			// _stablo_u_tabelu(kod, kor->u.p.l, (bin<<1),   n+1);
			// _stablo_u_tabelu(kod, kor->u.p.d, (bin<<1)+1, n+1);
			return _stablo_u_tabelu(kod, kor->u.p.l, bin+(0<<n), n+1) && 
				   _stablo_u_tabelu(kod, kor->u.p.d, bin+(1<<n), n+1);
	} else {
		if (n > 64)
			return 0;
		if (kor->tezina)
			kod[kor->u.bajt] = (struct kod){n, bin};
		else
			kod[kor->u.bajt] = (struct kod){0, 0};
	return 1;
	}
}
int stablo_u_tabelu(struct kod *kodovi, struct clan *koren) {
	return _stablo_u_tabelu(kodovi, koren, 0, 0);
}

int hfm_cheak_header(struct hfm_header *h) {
	return (h->magic[0] == HFM_MAGIC0)
		&& (h->magic[1] == HFM_MAGIC1)
		&& (h->magic[2] == HFM_MAGIC2)
		&& (h->magic[3] == HFM_MAGIC3);
}
