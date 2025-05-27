/*
 * mavg_int.c
 *
 *  Created on: Aug 12, 2023
 *      Author: hasankara.info
 */
#include "mavg_int.h"

#include <string.h>
#include <stdlib.h>
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

void (*cb_interrupt_dis)(void);
void (*cb_interrupt_en)(void);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

void mavg_cb_init(void (*en)(void), void (*dis)(void)) {
	cb_interrupt_en = en;
	cb_interrupt_dis = dis;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

void mavg_init_static(mavg_int_ts *p, void *buff, long qlen, uint8_t dsize,
		uint8_t is_signed) {
	p->is_dyn = 0;
	p->tot = 0;
	p->buff = buff;
	p->dsize = dsize;
	p->qlen = qlen;
	p->is_signed = is_signed;
	for (int i = 0; i < qlen * dsize; i++) {
		((uint8_t*) p->buff)[i] = 0;
	}
//	memset(p->buff, 0, sizeof(qlen * dsize));
	p->head = p->buff;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

mavg_int_ts* mavg_create(long qlen, uint8_t dsize, uint8_t is_signed) {
	mavg_int_ts *p = malloc(sizeof(mavg_int_ts) + qlen * dsize);
	if (!p)
		return 0;
	int size = sizeof(mavg_int_ts);
	size += (long) p;
	mavg_init_static(p, (void*) size, qlen, dsize, is_signed);
	p->is_dyn = 1;
	return p;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

void mavg_destroy(mavg_int_ts *p) {
	if (p->is_dyn)
		free(p);
}

/// ///\\\-\---\----\------\-------\--------\----------\-----------\-------------
long mavg_peek_avg(mavg_int_ts *p) {
	return p->tot / p->qlen;
}
long mavg_push_avg(mavg_int_ts *p, long in) {
	if (cb_interrupt_dis)
		cb_interrupt_dis();
	long long chead, cin;

	/*en eski deger cikarilir*/
	if (p->is_signed) {
		switch (p->dsize) {
		case 1:
			chead = *(char*) p->head;
			cin = (char) in;
			*(char*) p->head = cin;
			break;
		case 2:
			chead = *(short*) p->head;
			cin = (short) in;
			*(short*) p->head = cin;
			break;
		case 4:
			chead = *(long*) p->head;
			cin = (long) in;
			*(long*) p->head = cin;
			break;
		default:
			goto end;
		}
	} else {
		switch (p->dsize) {
		case 1:
			chead = *(unsigned char*) p->head;
			cin = (unsigned char) in;
			*(unsigned char*) p->head = cin;
			break;
		case 2:
			chead = *(unsigned short*) p->head;
			cin = (unsigned short) in;
			*(unsigned short*) p->head = cin;
			break;
		case 4:
			chead = *(unsigned long*) p->head;
			cin = (unsigned long) in;
			*(unsigned long*) p->head = cin;
			break;
		default:
			goto end;
		}
	}
	p->tot -= chead;
	/*en yeni deger eklenir*/
	p->tot += cin;

	p->head += p->dsize;

	long limit = p->qlen * p->dsize;
	limit += (long) p->buff;
	if ((long) p->head >= limit) {
		p->head = p->buff;
	}
	end: if (cb_interrupt_en)
		cb_interrupt_en();
	return mavg_peek_avg(p);
}

