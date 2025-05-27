/*****************************************************************
 * @File					:	hybos.h
 * @Author				:	Hasan KARA
 * @Revision				:	1
 * @Date					:	09.11.2014 Pazar, 00:45
 * @Brief					:	hybos base predefines 
 * @Attention			:
 *
 * <h2><center>&copy; COPYRIGHT(c) hasankara.info</center></h2>
 ***************************************************************/

#ifndef _HYBOS_H
#define _HYBOS_H

//\\\_\__\___\____\_____\______
//\\\			DEFINES
//\\\_\__\___\____\_____\______
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

#define _mcomp                  __cmp->

#define hyb_begin		hyb_ts *__cmp = hyb; _mbegin

#define _mtbase _mcomp tbase
#define _mstt _mcomp state
#define _mmsg _mcomp msg

#define __nState (__LINE__ & 0xffff)

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define _mbegin										\
	{															\
		if (_mmsg == hyb_msg_end)				\
			goto __hyb_end;							\
		if (_mmsg == hyb_msg_init)				\
		{														\
			_mstt = (__nState);						\
			_mmsg = hyb_msg_busy;				\
		}														\
	}															\
	switch (_mstt)										\
	{															\
	case (__nState):

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define hyb_set(x)										\
	do{															\
		_mstt = (__nState);							\
		x;														\
	case (__nState):;									\
	}while(0)
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define hyb_return(msg)							\
	{															\
		_mmsg = (msg);								\
		return ;												\
	}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define hyb_break(msg)								\
	{															\
		_mmsg = (msg);								\
		return ;												\
	}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define hyb_end										\
		hyb_return(hyb_msg_end);				\
	default:	hyb_return(hyb_msg_free);		\
		}														\
		{	__hyb_end:									\
			_mmsg = hyb_msg_end;				\
		}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define hyb_yield_(msg) hyb_set(hyb_break(msg))
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define hyb_yield() hyb_yield_(hyb_msg_yield)
#define hyb_suspend() hyb_yield_(hyb_msg_suspend)	
#define hyb_wait(val) _mtbase = hyb_get_tick() + val; hyb_yield_(hyb_msg_wait)
#define hyb_delay(val) _mtbase = hyb_get_tick() + val; hyb_yield_(hyb_msg_delayed)
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define hyb_queue_send_wait(queue, item,  priority, maxwait)							\
	{																												\
		hyb_get_sgnparam() = item;																	\
		hyb_ret_te ret = hyb_queue_send(queue, hyb_get_sgnparam(), priority);	\
		if (ret != hyb_ret_ok)																				\
		{																											\
			hyb_relation_create(queue, hyb);														\
			hyb_wait(maxwait);																			\
		}																											\
	}

#define hyb_queue_recv_wait(queue, maxwait)												\
	{																												\
		hyb_ret_te ret = hyb_queue_recv(queue, &hyb->sgn_param);					\
		if (ret != hyb_ret_ok)																				\
		{																											\
			hyb_relation_create(queue, hyb);														\
			hyb_wait(maxwait);																			\
		}																											\
	}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#endif /* _HYBOS_H */
