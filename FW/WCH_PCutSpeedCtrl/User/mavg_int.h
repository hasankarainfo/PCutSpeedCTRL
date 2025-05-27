/*
 * mavg_int.h
 *
 *  Created on: Aug 12, 2023
 *      Author: hass
 */

#ifndef USER_MAVG_INT_H_
#define USER_MAVG_INT_H_
#include "stdlib.h"
#include "stdint.h"

typedef struct {
	long qlen;
	uint8_t dsize;
	uint8_t is_signed :1;
	uint8_t is_dyn :1;

	long long tot;
	void *head;

	void *buff;

} mavg_int_ts;
extern void mavg_cb_init(void (*en)(void), void (*dis)(void));

extern void mavg_init_static(mavg_int_ts *p, void *buff, long qlen,
		uint8_t dsize, uint8_t is_signed);
extern mavg_int_ts* mavg_create(long qlen, uint8_t dsize, uint8_t is_signed);
extern void mavg_destroy(mavg_int_ts *p);

extern long mavg_peek_avg(mavg_int_ts *p);
extern long mavg_push_avg(mavg_int_ts *p, long in);

#endif /* USER_MAVG_INT_H_ */
