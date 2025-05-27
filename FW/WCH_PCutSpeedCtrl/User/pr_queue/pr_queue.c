/*****************************************************************
 * @File					:	pr_queue.c
 * @Author				:	Hasan KARA
 * @Revision				:	1
 * @Date					:	24 NISAN 2019 CUMA, 15:52
 * @Brief					:	priority handled queue operation headers
 * @Attention			:
 *
 * <h2><center>&copy; COPYRIGHT(c) hasankara.info</center></h2>
 ***************************************************************/

//\\\_\__\___\____\_____\______
//\\\			INCLUDES
//\\\_\__\___\____\_____\______
//\\\ @addtogroup
//\\\ @{
#include "pr_queue.h"
//\\\ @}

//\\\_\__\___\____\_____\______
//\\\			DEFINES
//\\\_\__\___\____\_____\______
//\\\ @addtogroup
//\\\ @{
#define RESV_QN_QTY 100
//\\\ @}

//\\\_\__\___\____\_____\______
//\\\			VARIABLES
//\\\_\__\___\____\_____\______
void (*_enter_critical)(void);
void (*_exit_critical)(void);

void* (*_memory_allocation)(long);
void (*_memory_free)(void*);

//\\\_\__\___\____\_____\______
//\\\			STATIC FUNCTIONS
//\\\_\__\___\____\_____\______
//\\\ @addtogroup
//\\\ @{

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
static void queue_putprev(qnode_ts *root, qnode_ts *node) {
	node->next = root;
	node->prev = root->prev;
	if (root == root->queue->first) {
		root->queue->first = node;
	}
	if (root->prev)
		root->prev->next = node;
	root->prev = node;
	node->queue = root->queue;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
static void queue_putnext(qnode_ts *root, qnode_ts *node) {
	node->prev = root;
	node->next = root->next;
	if (root == root->queue->last) {
		root->queue->last = node;
	}
	if (root->next)
		root->next->prev = node;
	root->next = node;
	node->queue = root->queue;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
static qnode_is_te qnode_is(qnode_ts *node) {
	if (node == 0)
		return qnode_is_not; /*mutlak hata*/

	if ((node)->queue == 0) {
		/*eleman kuyrukta degil*/
		if ((node)->next == 0 && (node)->prev == 0)
			return qnode_is_free;
		return qnode_is_not; /*mutlak hata*/
	} else {
		/*eleman kuyrukta*/
		if ((node)->prev == 0) {
			if ((node)->next == 0)
				return qnode_is_first_last; /*kuyrugun ilk ve son elemani(tek elemani)*/
			else
				return qnode_is_first; /*kuyrugun ilk elemani*/
		} else {
			if ((node)->next == 0)
				return qnode_is_last; /*kuyrugun son elemani*/
			else
				return qnode_is_mid; /*kuyrugun ara elemani*/
		}
	}
}
//\\\ @}

//\\\_\__\___\____\_____\______
//\\\			GLOBAL FUNCTIONS
//\\\_\__\___\____\_____\______

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void queue_cb_init_critical(void *enter_cb, void *exit_cb) {
	_enter_critical = enter_cb;
	_exit_critical = exit_cb;
}

void queue_cb_init_dynamic_memory(void *malloc, void *free) {
	_memory_allocation = malloc;
	_memory_free = free;
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------

qnode_ts* qnode_peek_next(qnode_ts *qn) {
	if (qn)
		return qn->next;
	return 0;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
qnode_ts* qnode_peek_prev(qnode_ts *qn) {
	if (qn)
		return qn->prev;
	return 0;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void qnode_set_obj(qnode_ts *qn, void *obj) {
	if (qn)
		qn->obj = obj;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void* qnode_get_obj(qnode_ts *qn) {
	if (qn)
		return qn->obj;
	else
		return 0;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void qnode_set_prio(qnode_ts *qn, qnode_prio_t prio) {
	if (qn)
		qn->prio = prio;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
qnode_prio_t qnode_get_prio(qnode_ts *qn) {
	if (qn)
		return qn->prio;
	else
		return 0;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void qnode_set_mostprio(qnode_ts *qn, bool mostprio) {
	if (qn)
		qn->mostprio = mostprio;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
bool qnode_get_mostprio(qnode_ts *qn) {
	if (qn)
		return qn->mostprio;
	else
		return 0;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void qnode_set_desc(qnode_ts *qn, unsigned char desc) {
	if (qn)
		qn->desc = desc;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
unsigned char qnode_get_desc(qnode_ts *qn) {
	if (qn)
		return qn->desc;
	else
		return 0;
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
int queue_get_mostprio(queue_ts *q) {
	if (q)
		return q->first->mostprio;
	else
		return 0;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

queue_size_t queue_get_size(queue_ts *q) {
	if (q)
		return q->size;
	else
		return 0;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

qnode_ret_te queue_send_node(queue_ts *q, qnode_ts *qn) {
	if (q == 0 || qn == 0)
		return qnode_ret_err;
	qnode_ret_te ret = qnode_ret_err;
	_enter_critical();
	do {
		/* @note : node serbest degilse ekleme yapilamaz*/
		/* once node serbest birakilmalidir*/
		if (qnode_is(qn) != qnode_is_free)
			break;

		qn->queue = q;
		q->size++;
		/* kuyrugun ilk veya son elemani bos ise, eklenecek eleman 
		 * kuyrugun ilk ve son elemani olur*/
		if (q->first == 0 || q->last == 0) {
			q->first = qn;
			q->last = qn;
		} else {
			qnode_ts *tmp;
			qnode_prio_t pr = qnode_get_prio(qn);
			qn->mostprio = 0;

			/*dusuge yakinsa last dan aranmaya baslanir*/
			for (tmp = q->last; tmp->prio < pr && tmp->prev != 0;
					tmp = tmp->prev) {
			}
			if (tmp->prio >= pr) {
				/* kuyruktan elemanlar, ilk ve prev den cekilir. Bu yuzden,
				 *esit oncelikliler nexte atilarak, kendi aralarinda
				 *kuyruga eklenme zaman onceliklendirmesi saglanir.*/
				queue_putnext(tmp, qn);
			} else {
				queue_putprev(tmp, qn);
			}
		}
		ret = qnode_ret_ok;
	} while (0);
	_exit_critical();

	return ret;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
qnode_ret_te queue_sendforce_node(queue_ts *q, qnode_ts *qn) {
	if (q == 0 || qn == 0)
		return qnode_ret_err;
	qnode_ret_te ret = qnode_ret_err;
	_enter_critical();
	do {
		if (qnode_release(qn) != qnode_ret_ok)
			break;
		if (queue_send_node(q, qn) != qnode_ret_ok)
			break;
		ret = qnode_ret_ok;
	} while (0);
	_exit_critical();
	return ret;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
qnode_ts* queue_peek_node(queue_ts *q) {
	if (q == 0)
		return 0;
	_enter_critical();
	qnode_ts *qn = 0;
	do {
		if (q->first == 0)
			break;
		qn = q->first;
	} while (0);
	_exit_critical();

	return qn;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
qnode_ts* queue_recv_node(queue_ts *q) {
	if (q == 0)
		return 0;

	qnode_ts *qn = 0;
	qn = queue_peek_node(q);
	if (qn != 0) {
		qnode_release(qn);
	}
	return qn;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
qnode_ret_te qnode_release(qnode_ts *qn) {
	if (qn == 0)
		return qnode_ret_err; /*mutlak hata*/

	_enter_critical();
	qnode_ret_te ret = qnode_ret_ok;

	qnode_is_te ret_is = qnode_is(qn);
	switch (ret_is) {
	case qnode_is_free: /* zaten serbest*/
		break;
	case qnode_is_first_last: /*kuyrugun ilk ve son elemani(tek elemani)*/
		qn->queue->last = 0;
		qn->queue->first = 0;
		break;
	case qnode_is_first: /*kuyrugun ilk elemani*/
		qn->queue->first = qn->next;
		qn->next->prev = qn->prev;
		break;
	case qnode_is_last: /*kuyrugun son elemani*/
		qn->queue->last = qn->prev;
		qn->prev->next = qn->next;
		break;

	case qnode_is_mid: /*kuyrugun ara elemani*/
		qn->prev->next = qn->next;
		qn->next->prev = qn->prev;
		break;

	case qnode_is_not:
	default:
		ret = qnode_ret_err;
	}

	if (ret == qnode_ret_ok && ret_is != qnode_is_free) {
		qn->queue->size--;
		qn->next = 0;
		qn->prev = 0;
		qn->queue = 0;
	}
	_exit_critical();

	return ret;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
qnode_ts* qnode_create(void *obj, qnode_prio_t prio, bool mostprio) {
	qnode_ts *qn = _memory_allocation(sizeof(qnode_ts));
	if (qn != 0) {
		nullset(qn, qnode_ts);
		qnode_set_obj(qn, obj);
		qnode_set_prio(qn, prio);
		qnode_set_mostprio(qn, mostprio);

		qnode_set_desc(qn, 0);
		qn->is_dyn = true;
	}
	return qn;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
queue_ts* queue_create() {
	queue_ts *q = _memory_allocation(sizeof(queue_ts));
	if (q != 0) {
		nullset(q, queue_ts);
	}
	return q;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void* queue_recv_obj(queue_ts *q) {
	if (q == 0)
		return 0;
	_enter_critical();
	void *obj = 0;
	do {
		if (q->size < 1)
			break;
		qnode_ts *qn = queue_recv_node(q);
		if (qn == 0)
			break;
		obj = qnode_get_obj(qn);
		/* kuyruk icinde desc bilgisinin saglandigi degisken olusturulur
		 * burada o degisken guncellenir*/
		if (qn->is_dyn) {
			_memory_free(qn);
			break;
		} else {
			/* static node serbest birakilmaz kullanicinin kendi takip etmesi gerekir*/
			/* static node kuyrukta kullaniliyorsa, obje send ve receive fonksiyonlari
			 * ile bu kuyruk uzerinde islem yapilmasi onerilmez.*/
		}

	} while (0);
	_exit_critical();
	return obj;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
qnode_ts* queue_send_obj(queue_ts *q, void *obj, qnode_prio_t prio,
		bool mostprio) {
	if (q == 0)
		return 0;
	_enter_critical();
	qnode_ts *qn = 0;
	do {
		qn = qnode_create(obj, prio, mostprio);
		if (qn == 0)
			break;
		if (queue_send_node(q, qn) != qnode_ret_ok)
			break;
	} while (0);
	_exit_critical();
	return qn;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

/*************************** END OF FILE *************************
 ****************** (C) COPYRIGHT hasankara.info ******************/
