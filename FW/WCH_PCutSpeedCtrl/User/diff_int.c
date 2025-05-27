
/**
 * @file diff_int.c
 *
 *@Date: 30.10.2020		Friday	03:08
 *@Author: Hasan KARA
 */


/*********************
 *      INCLUDES
 *********************/
#include "diff_int.h"

/**********************
 *      GLOBAL VAR
 **********************/
void (*cb_diff_yield)(int val);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static diff_max_t _raw_val(diff_ts *p, diff_max_t val)
{
	/* isaretsiz deger alinir*/
	if (p->en_signed && (val & ((uint32_t)1 << (p->bit_size ))))
	{
		/* negatif ise*/
		val |= -1 << p->bit_size;
	}
	else
	{
		/* pozitif ise*/
		val &= ~(-1 << p->bit_size);
	}
	return val;
}
//_/__/___/____/_____/______/_______/________/
/* gurultu varsa onceki kayit edilmis deger cikisa aktarili*/

diff_max_t diff_val(diff_ts *p)
{
	diff_max_t raw =  _raw_val(p, *(diff_max_t*)p->p_src);
//	diff_max_t diff =  raw  - p->v_last;
	diff_max_t diff =  _raw_val (p, raw  - p->v_last);


	

	p->curr_dir = diff_dir(diff);

	if (p->prv_dir != p->curr_dir)
	{
		if (diff_abs(diff) >= p->rev_thresh)
		{
//			p->v_rec +=  diff_sgn(diff)*(p->rev_thresh);
			diff_add(p, diff_sgn(diff)*(p->rev_thresh));
			
			p->prv_dir = p->curr_dir;
			p->v_last =   (raw);	
				
			diff = 0;	
		}
		
	}
	else
	{
		p->v_last =  (raw);	
		diff = 0;	
	}
	
//	return (raw - diff) + p->rev_thresh / 2;
	diff_max_t val  = _raw_val(p, (raw - diff) + p->rev_thresh / 2);

	return val;
	//	return ((raw - diff) + diff_sgn(diff)*(p->rev_thresh / 2) );
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* Fark degerine yeni deger ekler*/
 void diff_add(diff_ts *p, diff_max_t val)
{
	p->v_rec = _raw_val(p, p->v_rec + val) ;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* fark degeri dondurur*/
 diff_max_t diff_peekraw(diff_ts *p)
{
//	return (_raw_val(p, *(diff_max_t*)p->p_src) - p->v_rec);
	 return _raw_val(p, _raw_val(p, *(diff_max_t*)p->p_src) - p->v_rec);
//	 return _raw_val(p, *(diff_max_t*)p->p_src - p->v_rec) ;
}


/* fark degeri dondurur*/
 diff_max_t diff_peek(diff_ts *p)
{
//	return (diff_val(p) - p->v_rec );
	 return _raw_val(p, diff_val(p) - p->v_rec);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
 diff_max_t diff_getraw(diff_ts *p)
{
	diff_max_t diff = diff_peekraw(p);
	diff_add(p, diff);
	return (diff);
}

/* Get All Differantial Values*/
 diff_max_t diff_get(diff_ts *p)
{
	 diff_max_t diff = diff_peek(p);
	diff_add(p, diff);
	return (diff);
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* tum fark degeri temizlenir, kalan degerde temizlenir*/
 void diff_reset(diff_ts *p)
{
	p->v_rec = diff_val(p);
	p->prv_dir = p->curr_dir;
}

 ///\\\-\---\----\------\-------\--------\----------\-----------\-------------
  void diff_set(diff_ts *p, diff_max_t offset )
 {
 	diff_reset(p);
 	diff_add(p, offset);
 }
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
 bool diff_is(diff_ts *p)
{
	return ((diff_peek(p) != 0) ? true : false);
}

 ///\\\-\---\----\------\-------\--------\----------\-----------\-------------
  void diff_wait(diff_ts *p, diff_max_t val )
 {
	diff_set(p, val );/*Bu satira gerek olmayabilir*/
	while(diff_peek(p)<0) {if(cb_diff_yield != 0 )cb_diff_yield(val);}
 }
  ///\\\-\---\----\------\-------\--------\----------\-----------\-------------
  void diff_en_rev_tresh(diff_ts *p, diff_max_t rev_thresh_val)
  {
	  p->rev_thresh = rev_thresh_val;
  }
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
  void diff_init_all(
 	diff_ts *p,
 	void *src,
 	uint8_t bit_size,
 	bool is_signed,
	diff_max_t rev_thresh_val
 	)
  {
	diff_init(p,  src, bit_size, is_signed);
	diff_en_rev_tresh(p, rev_thresh_val);
  }
 void diff_init(
	diff_ts *p,
	void *src,
	uint8_t bit_size,
	bool is_signed
	)
{
	p->p_src =	src;
	p->bit_size = bit_size - 1 ;

	p->rev_thresh = 0;
	
	p->v_last = _raw_val(p, *(diff_max_t*)p->p_src);
	p->prv_dir = 0;
	
	p->en_signed = is_signed;
	
	diff_reset(p);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------



