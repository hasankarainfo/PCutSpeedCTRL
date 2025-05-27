/*****************************************************************
  * @File					:	pr_queue.h
  * @Author				:	Hasan KARA
  * @Revision				:	1
  * @Date					:	24 NISAN 2019 CUMA, 15:52
  * @Brief					:	priority handled queue operations headers
  * @Attention			:
  *
  * <h2><center>&copy; COPYRIGHT(c) hasankara.info </center></h2>
  ***************************************************************/
#ifndef _PR_QUEUE_H_
#define _PR_QUEUE_H_

#include <stdbool.h>
//\\\_\__\___\____\_____\______
//\\\			DEFINES
//\\\_\__\___\____\_____\______

#define nullset(_struct, type)             \
	for (int i = 0; i < sizeof(type); i++) \
	((char *)_struct)[i] = 0

//\\\_\__\___\____\_____\______
//\\\			TYPEDEFS
//\\\_\__\___\____\_____\______
//\\\ @addtogroup
//\\\ @{
typedef enum
{
	/*\\\* @brief: islem tamamlandi */
	qnode_ret_ok = 0,
	/*\\\* @brief: hata olustu*/
	qnode_ret_err = 1
} qnode_ret_te;

typedef enum
{
	/*\\\* @brief: node bulunamadi */
	qnode_is_not = 0,
	/*\\\* @brief: node serbest durumunda */
	qnode_is_free = 1,
	/*\\\* @brief: node iliskili kuyrugun tek elemani durumunda */
	qnode_is_first_last = 2,
	/*\\\* @brief: node iliskili kuyrugun ilk elemani durumunda */
	qnode_is_first = 3,
	/*\\\* @brief: node iliskili kuyrugun son elemani durumunda */
	qnode_is_last = 4,
	/*\\\* @brief: node iliskili kuyrugun ara elemani durumunda */
	qnode_is_mid = 5,

} qnode_is_te;
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
typedef long qnode_prio_t;
typedef unsigned long queue_size_t;

typedef struct
{
	/* \\\* @brief		:	kuyrugun ilk elemani*/
	struct qnode_s *first;
	/*\\\* @brief		:	kuyrugun son elemani*/
	struct qnode_s *last;
	/*\\\* @brief		:	kuyrukta ki guncel node sayisi*/
	queue_size_t size;
} queue_ts;
///\\\-\---\----\------\-------\--------\----------\-----------\-------------


typedef struct qnode_s
{
	/*\\\* @brief	:	nodun tasidigi objedir*/
	void *obj;

	struct qnode_s *prev;
	struct qnode_s *next;

	/*\\\* @brief		:	nodun yerlestirildigi kuyruktur*/
	queue_ts *queue;

	/*
	  \\\* @brief		:	ilgili objenin onceligi takip edilir.
	  \\\*		@arg		:	0				:	lower prio
	  \\\*		@arg		:	[1 , 0xffffffff]	:	most prio
	  */
	qnode_prio_t prio;

	/*\\\* @brief		:	node obje tanimlayicisidir.	*/
	unsigned char desc;

	/*
	  \\\* @brief		:	esit oncelikli node lar arasinda tercih modu 
	  \\\*		@arg		:	0	:	dusuk onceliklidir
	  \\\*		@arg		:	1	:	yuksek onceliklidir
	  */
	bool mostprio : 1;

	/*
	  \\\* @brief		:	ilgili objenin dinamik bellek ozelligi takip edilir.
	  \\\*		@arg		:	qnode_static_e	:	static
	  \\\*		@arg		:	qnode_dynamic_e	:	dinamik
	  */
	bool is_dyn : 1;
} qnode_ts;



//\\\ @}

//\\\_\__\___\____\_____\______
//\\\			VARIABLES
//\\\_\__\___\____\_____\______
 extern void (*_enter_critical)(void);
 extern void (*_exit_critical)(void);
  
 extern void* (*_memory_allocation)(long); 
 extern void (*_memory_free)(void*); 

//\\\_\__\___\____\_____\______
//\\\			FUNCTIONS
//\\\_\__\___\____\_____\______
//\\\ @addtogroup
//\\\ @{

/*
  \\\* @brief		:	node a bagli sonraki nodu sorgular
  \\\* @param	:	node belirteci
  \\\*					
  \\\* @retval		:	sonraki node
*/
extern qnode_ts *qnode_peek_next(qnode_ts *qn);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	node a bagli onceki nodu sorgular
  \\\* @param	:	node belirteci
  \\\*					
  \\\* @retval		:	onceki node
*/
extern qnode_ts *qnode_peek_prev(qnode_ts *qn);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	node objesinin guncellenmesini saglar
  \\\* @param	:	node belirteci
  \\\* @param	:  yerlestirilecek obje
  \\\*					
  \\\* @retval		:	void
*/
extern void qnode_set_obj(qnode_ts *qn, void *obj);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	node objesinin alinmasini saglar
  \\\* @param	:	node belirteci
  \\\*					
  \\\* @retval		:	obje
*/
extern void *qnode_get_obj(qnode_ts *qn);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	node nin oncelik degerini gunceller
  \\\* @param	:	node belirteci
  \\\* @param	:	oncelik degeri
  \\\*					
  \\\* @retval		:	void
*/
extern void qnode_set_prio(qnode_ts *qn, qnode_prio_t prio);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	Node daha oncelikli ozellik sorgular
  \\\* @param	:	Node
  \\\*					
  \\\* @retval		:	Daha oncelikli ozellik durumu
*/
extern qnode_prio_t qnode_get_prio(qnode_ts *qn);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	Node daha oncelikli ozellik gunceller
  \\\* @param	:	Node
  \\\* @param	:	Daha oncelikli ozellik degeri
  \\\*					
  \\\* @retval		:	void
*/
extern void qnode_set_mostprio(qnode_ts *qn, bool mostprio);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	node nin daha oncelikli ozelligini sorgular
  \\\* @param	:	node belirteci
  \\\*					
  \\\* @retval		:	node daha oncelikli durumu
*/
extern bool qnode_get_mostprio(qnode_ts *qn);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	node nin tanimlayici degerini gunceller
  \\\* @param	:	node belirteci
  \\\* @param	:	tanimlayici degeri
  \\\*					
  \\\* @retval		:	void
*/
extern void qnode_set_desc(qnode_ts *qn, unsigned char desc);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	node nin tanimlayici degerini sorgular
  \\\* @param	:	node belirteci
  \\\*					
  \\\* @retval		:	node tanimlayici degeri
*/
extern unsigned char qnode_get_desc(qnode_ts *qn);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

/*
  \\\* @brief		:	kuyruktan, en oncelikli node nin, oncelik degerini verir
  \\\* @param	:	kuyruk adres
  \\\*		
  \\\* @retval		:	kuyruktaki en oncelikli nodenin oncelik degeri
*/
extern int queue_get_mostprio(queue_ts *q);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

extern queue_size_t queue_get_size(queue_ts *q);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

/*
  \\\* @brief		:	kuyruk islemleri kritik durumlarda
  \\\*						cagirilacak kritik callback fonksiyonlari gunceller.
  \\\* @param	:	kritik baslangicinda cagrilacak fonksiyon belirteci
  \\\* @param	:	kritik bitisinde cagrilacak fonksiyon belirteci
  \\\*		
  \\\* @retval		:	void
*/
extern void queue_cb_init_critical(void *enter_cb, void *exit_cb);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	kuyruk islemleri dinamik bellek operasyonlari
  \\\*						icin kritik callback fonksiyonlari gunceller.
  \\\* @param	:	dinamik bellek alan ayiran fonksiyon belirteci
  \\\* @param	:	dinamik bellek serbest birakan fonksiyon belirteci
  \\\*		
  \\\* @retval		:	void
*/
extern void queue_cb_init_dynamic_memory(void *malloc, void *free);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

/*
  \\\* @brief		:	kuyrugun sonuna node iliskilendirilir.
  \\\* @note		:	islemin gerceklesmesi icin node sebest olmalidir.
  \\\* @param	:	kuyruk belirteci
  \\\* @param	:	eklenecek node nin belirteci
  \\\*		
  \\\* @retval		:	durum mesaji
  \\\*		@arg		:	qnode_ret_ok : ekleme tamamlandi
  \\\*		@arg		:	qnode_ret_err : hata
*/
extern qnode_ret_te queue_send_node(queue_ts *q, qnode_ts *node);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	kuyrugun sonuna node ekler. ilgili node 
  \\\*						serbest degil ise oncelikle serbest hale getirilir.
  \\\* @param	:	kuyruk belirteci
  \\\* @param	:	eklenecek node nin belirteci
  \\\*		
  \\\* @retval		:	durum mesaji
  \\\*		@arg		:	qnode_ret_ok : ekleme tamamlandi
  \\\*		@arg		:	qnode_ret_err : hata
*/
extern qnode_ret_te queue_sendforce_node(queue_ts *q, qnode_ts *qn);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	kuyruktan, en oncelikli node sorgulanir
  \\\* @note		:	Sorgulanan node kuyrukla ilisigi devam eder
  \\\* @param	:	kuyruk
  \\\*		
  \\\* @retval		:	en oncelikli node
  \\\*		@arg		:	0 : hata mesaji
*/
extern qnode_ts *queue_peek_node(queue_ts *q);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	kuyruktan, en oncelikli node alinir
  \\\* @param	:	kuyruk
  \\\*		
  \\\* @retval		:	en oncelikli node
  \\\*		@arg		:	0 : hata mesaji
*/
extern qnode_ts *queue_recv_node(queue_ts *q);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	node serbest birakilir
  \\\* @note		:	herhangi bir kuyruk ile ilisigi kalmaz
  \\\* @param	:	node belirteci
  \\\*		
  \\\* @retval		:	durum mesaji
  \\\*		@arg		:	qnode_ret_ok : node serbest
  \\\*		@arg		:	qnode_ret_err : hata
*/
extern qnode_ret_te qnode_release(qnode_ts *node);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	Serbest bellek alanindan node olusturur init eder
  \\\* @param	:	Node icine yuklenecek obje
  \\\* @param	:	Node oncelik degeri
  \\\* @param	:	Node daha oncelikli degeri
  \\\*		
  \\\* @retval		:	Olusturulan node
  \\\*		@arg		:	0 : hata mesaji
*/
extern qnode_ts *qnode_create(void *obj, qnode_prio_t prio, bool mostprio);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	Serbest bellek alanindan kuyruk olusturur init eder
  \\\*		
  \\\* @retval		:	Olusturulan kuyruk
  \\\*		@arg		:	0 : hata mesaji
*/
extern queue_ts *queue_create();
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	Kuyruktan obje alinmasini saglar
  \\\* @note		:	Objeyi tasiyan node, bos node havuzuna geri yuklenir
  \\\* @param	:	Kuyruk
  \\\*		
  \\\* @retval		:	Alinan obje
  \\\*		@arg		:	0 : hata olabilir. get_lasterr ile teyit edilebilir.
*/
extern void* queue_recv_obj(queue_ts *q);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*
  \\\* @brief		:	Kuyruga obje yerlestirilmesini saglar
  \\\* @note		:	Bos node havuzundan node alir, init ederek kuyruga yerlestirir
  \\\* @param	:	Kuyruk
  \\\* @param	:	Yerlestirilecek obje
  \\\* @param	:	Oncelik degeri
  \\\* @param	:	Daha oncelikli deger
  \\\*		
  \\\* @retval		:	Objeyi tasiyan node
  \\\*		@arg		:	0 : hata
*/
extern qnode_ts *queue_send_obj(queue_ts *q, void *obj, qnode_prio_t prio, bool mostprio);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

//\\\ @}

#endif
/*************************** END OF FILE *************************
  ****************** (C) COPYRIGHT hasankara.info ******************/
