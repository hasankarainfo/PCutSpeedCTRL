#pragma once
/**
 * @file diff_int.h
 *
 *@Date: 30.10.2020		Friday	03:08
 *@Author: Hasan KARA
 */

/*********************
 *      INCLUDES
 *********************/
#include "stdint.h"
#include "stdbool.h"

/**********************
 *      GLOBAL VAR
 **********************/
extern void (*cb_diff_yield)(int val);

/**********************
 *      TYPEDEFS
 **********************/
typedef long diff_max_t;

typedef struct {
	diff_max_t rev_thresh;
	diff_max_t v_last;

	/* kaynak degiskenin bit uzunlugudur*/
	uint8_t bit_size;
	/* kaynak degisken isaretli oldugu belirtir*/
	uint8_t en_signed :1;

	uint8_t prv_dir :1;
	uint8_t curr_dir :1;

	/* kaynak degisken pointeri*/
	void *p_src;
	/* kaydedilen deger*/
	diff_max_t v_rec;
} diff_ts;

enum {
	en_signed = true, dis_signed = false,

	en_rev_reset = true, dis_rev_reset = false,
};

/**********************
 *  MACROS
 **********************/

#define val_limit(val,  min, max) ((val > max) ? (max) : ((val < min) ? (min) : (val )))

#define diff_abs(VAL)((VAL < 0) ? (-VAL) : (VAL))

#define diff_dir(val)	((val>0) ? (1): (0))
#define diff_sgn(val)	((val>0) ? (1):(-1))

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Normal deger dondurur*/
extern diff_max_t diff_val(diff_ts *p);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

/* Fark degerine yeni deger ekler*/
extern void diff_add(diff_ts *p, diff_max_t val);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* fark degeri dondurur*/
extern diff_max_t diff_peekraw(diff_ts *p);

/* fark degeri dondurur*/
extern diff_max_t diff_peek(diff_ts *p);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
extern diff_max_t diff_getraw(diff_ts *p);

/* Get All Differantial Values*/
extern diff_max_t diff_get(diff_ts *p);

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* tum fark degeri temizlenir, kalan degerde temizlenir*/
extern void diff_reset(diff_ts *p);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

extern void diff_set(diff_ts *p, diff_max_t offset);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

extern bool diff_is(diff_ts *p);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

extern void diff_wait(diff_ts *p, diff_max_t val);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
extern void diff_en_rev_tresh(diff_ts *p, diff_max_t rev_thresh_val);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

extern void diff_init_all(diff_ts *p, void *src, uint8_t bit_size,
bool is_signed, diff_max_t rev_thresh_val);

/*********************************************************************
 * @fn      diff_init_detail
 *
 * @brief   Initializes diff struct parameters.
 *
 * @param   p - diff struct pointer.
 *          src - register pointer to be followed
 *          bit_size - bit length of register pointer, including sign bit
 *          is_signed - Indicates that the sign bit of the register pointer is
 *          rev_tresh - It is the hysteresis value required to update the value to be read.
 * @return  none
 */
extern void diff_init(diff_ts *p, void *src, uint8_t bit_size,
bool is_signed);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define is_signed(src)	({typeof(src) _t=-1; (_t < 0 ? true : false); })
/*********************************************************************
 * @fn      diff_init
 *
 * @brief   Initializes diff struct parameters some default values.
 *
 * @param   p - diff struct pointer.
 *          src - register pointer to be followed
 * @return  none */
#define diff_init_def(p, src) \
		diff_init(p, src, sizeof(*(src))*8 ,  is_signed(*(src)) )
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

