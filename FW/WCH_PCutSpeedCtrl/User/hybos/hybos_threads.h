
/*****************************************************************
  * @File					:	hybos_threads.h
  * @Author				:	Hasan KARA
  * @Revision				:	1
  * @Date					:	24 NISAN 2019 CUMA, 15:52
  * @Brief					:	hybos thread operation headers
  * @Attention			:
  *
  * <h2><center>&copy; COPYRIGHT(c) hasankara.info</center></h2>
  ***************************************************************/

#ifndef _HYBOS_THREADS_H_
#define _HYBOS_THREADS_H_
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
//\\\_\__\___\____\_____\______
//\\\			INCLUDES
//\\\_\__\___\____\_____\______

#include "hybos_base.h"
#include "pr_queue.h"
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
//\\\_\__\___\____\_____\______
//\\\			DEFINES
//\\\_\__\___\____\_____\______
#define HYB_MAX_DELAY (0X7FFFFFFF - hyb_get_tick())
#define hyb_get_param() hyb->app_param
#define hyb_get_sgnparam() hyb->sgn_param
#define hyb_get_lasterr() hyb->totalerr
//\\\_\__\___\____\_____\______
//\\\			TYPEDEFS
//\\\_\__\___\____\_____\______
typedef enum
{
	hyb_ret_ok = 0x00,
	hyb_ret_error =0x01,
	hyb_ret_pending=0x02,
	hyb_ret_deadlock=0x04,
	hyb_ret_free=0x08,
} hyb_ret_te;

typedef enum
{
	hyb_msg_null = 0x0,
	hyb_msg_init = 0x1,
	hyb_msg_busy = 0x2,
	hyb_msg_yield = 0x3,
	hyb_msg_wait = 0x4,
	hyb_msg_delayed = 0x5,
	hyb_msg_suspend = 0x6,
	hyb_msg_end = 0x7,
	hyb_msg_exit = 0x8,
	hyb_msg_free = 0x9,
} hyb_msg_te;

typedef enum
{
	hyb_cmd_none,
	hyb_cmd_start,
	hyb_cmd_stop,
	hyb_cmd_restart,
	hyb_cmd_force_run,	
	hyb_cmd_run,
	hyb_cmd_free
	
} hyb_cmd_te;

typedef struct hyb_ts
{

	void (*app_cb)(struct hyb_ts *);
	void *app_param;

	void *sgn_param;

	queue_ts relations;
	qnode_prio_t prio;

	long tbase;

	unsigned short state;
	hyb_msg_te msg : 4;
	hyb_cmd_te pending : 3;
	hyb_cmd_te operation : 3;
	hyb_ret_te totalerr : 4;
	bool is_dyn : 1;
	bool run : 1;
	/* hyb_run fonksiyonu ile calistirilmasi kontrol edilir */
	bool force_run : 1;
	
} hyb_ts;

typedef struct
{
	qnode_ts qn_up, qn_down;
	hyb_ts *up, *down;
	bool is_dyn : 1;
	unsigned char carry_cnt : 2;
	
		/*run veya stop calistirilinca kullanilir*/
	bool up_link : 1;
	bool down_link : 1;

	/* force_run edilirken kullanilir*/
	bool up_force_run : 1;
	bool down_force_run : 1;

	/* handler calistirilirken kullanilir*/
	bool up_run : 1;
	bool down_run : 1;

	/* free calisitirilirken kullanilir*/
	bool up_free : 1;
	bool down_free : 1;
} hyb_relation_ts;



//\\\_\__\___\____\_____\______
//\\\			VARIABLES
//\\\_\__\___\____\_____\______
extern long (*hyb_get_tick)(void);

//\\\_\__\___\____\_____\______
//\\\			RULES FUNCTIONS
//\\\_\__\___\____\_____\______
extern void hyb_set_runnability(hyb_ts *hyb, bool force_run, bool run);
extern void hyb_set_rule_link(hyb_relation_ts *hr, bool up, bool down);
extern void hyb_set_rule_force_run(hyb_relation_ts *hr, bool up, bool down);
extern void hyb_set_rule_run(hyb_relation_ts *hr, bool up, bool down);
extern void hyb_set_rule_free(hyb_relation_ts *hr, bool up, bool down);
//\\\_\__\___\____\_____\______
//\\\			COMMAND FUNCTIONS
//\\\_\__\___\____\_____\______

extern hyb_ret_te hyb_start(hyb_ts *hyb, void *sgn_param);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
extern hyb_ret_te hyb_stop(hyb_ts *hyb);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
extern hyb_ret_te hyb_restart(hyb_ts *hyb);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
extern hyb_ret_te hyb_force_run(hyb_ts *hyb, void *sgn_param);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
extern hyb_ret_te hyb_run(hyb_ts *hyb, long *tick_min);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
extern hyb_ret_te hyb_free(hyb_ts *hyb);
//\\\_\__\___\____\_____\______
//\\\			NOTIFY FUNCTIONS
//\\\_\__\___\____\_____\______
extern hyb_ret_te hyb_notify(hyb_ts *hyb, void *sgn_param);
//\\\_\__\___\____\_____\______
//\\\			RAW FUNCTIONS
//\\\_\__\___\____\_____\______
extern void hyb_relation_break(hyb_relation_ts *hn);
extern hyb_ret_te hyb_relation_add(hyb_relation_ts *hn, hyb_ts *up, hyb_ts *down);
extern hyb_relation_ts *hyb_relation_create(hyb_ts *up, hyb_ts *down);

extern hyb_ret_te hyb_init(hyb_ts *hyb, void *func, void *param, unsigned long prio);
extern hyb_ts *hyb_create( void *func, void *param, long prio);
//\\\_\__\___\____\_____\______
//\\\			THREAD FUNCTIONS
//\\\_\__\___\____\_____\______
extern hyb_relation_ts *hyb_thread_add(hyb_ts *parent, hyb_ts *hyb);
extern hyb_ts *hyb_thread_create(hyb_ts *parent, void *func, void *param, long prio);
//\\\_\__\___\____\_____\______
//\\\			QUEUE FUNCTIONS
//\\\_\__\___\____\_____\______
extern hyb_relation_ts *hyb_queue_add(hyb_ts *parent, hyb_ts *hyb);
extern hyb_ts *hyb_queue_create(hyb_ts *parent, queue_size_t limit);
extern hyb_ret_te hyb_queue_send(hyb_ts *hyb, void *sgn_param, qnode_prio_t prio);
extern hyb_ret_te hyb_queue_recv(hyb_ts *hyb, void *sgn_param);
//\\\_\__\___\____\_____\______
//\\\			PORT FUNCTIONS
//\\\_\__\___\____\_____\______

extern void hyb_init_port();
extern hyb_ts *hyb_preemptive_create(hyb_ts *parent, char *name, int stack, int prio, int core);

#endif /*_HYBOS_THREADS_H_*/
