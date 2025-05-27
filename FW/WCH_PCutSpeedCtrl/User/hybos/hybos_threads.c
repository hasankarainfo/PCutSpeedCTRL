/*****************************************************************
 * @File					:	hybos_threads.c
 * @Author				:	Hasan KARA
 * @Revision				:	1
 * @Date					:	24 NISAN 2019 CUMA, 15:52
 * @Brief					:	hybos thread operation
 * @Attention			:
 *
 * <h2><center>&copy; COPYRIGHT(c) hasankara.info</center></h2>
 ***************************************************************/
//\\\_\__\___\____\_____\______
//\\\			INCLUDES
//\\\_\__\___\____\_____\______
#include "hybos_threads.h"

long (*hyb_get_tick)(void);

///\\\-\---\----\------\-------\--------\----------\-----------\-------------

//\\\_\__\___\____\_____\______
//\\\			STATIC FUNCTIONS
//\\\_\__\___\____\_____\______

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
static hyb_ret_te _hyb_command(hyb_ts *hyb, long *tmin, hyb_cmd_te cmd) {
	if (!hyb)
		return hyb_ret_error;

	hyb->totalerr = hyb_ret_ok;
	qnode_ts *qn = queue_peek_node(&hyb->relations);
	while (qn != 0) {
		hyb_ts *sub = 0;
		hyb_relation_ts *hn = qnode_get_obj(qn);
		bool isdown = false;
		if (hn) {
			if (qn == &hn->qn_up) {
				isdown = true;
				sub = hn->down;
			} else if (qn == &hn->qn_down) {
				isdown = false;
				sub = hn->up;
			}
		}
		qn = qnode_peek_next(qn);
		if (!sub)
			continue;

		{
			bool sub_command = false;
			switch ((int) cmd) {
			case hyb_cmd_start:
			case hyb_cmd_stop:
			case hyb_cmd_restart:
				if ((hn->down_link && !isdown) || (hn->up_link && isdown)) {
					sub->sgn_param = hyb->sgn_param;
					sub_command = true;
				}
				break;

			case hyb_cmd_force_run:
				if ((hn->down_force_run && !isdown)
						|| (hn->up_force_run && isdown)) {
					sub_command = true;
				}
				break;

			case hyb_cmd_run:
				if ((hn->down_run && !isdown) || (hn->up_run && isdown)) {
					sub_command = true;
				}
				break;
			case hyb_cmd_free:
				if ((hn->down_free && !isdown) || (hn->up_free && isdown)) {
					sub_command = true;
				}

				break;
			}
			if (!sub_command)
				continue;
		}
		/* DEADLOCK*/
		if (hn->carry_cnt >= 2) {
			hyb->totalerr |= hyb_ret_deadlock;
			continue;
		}
		hn->carry_cnt++;
		hyb_ret_te lasterr = _hyb_command(sub, tmin, cmd);
		if (lasterr != hyb_ret_free)
			hn->carry_cnt--;
		hyb->totalerr |= lasterr;
	}

	unsigned char pending_cnt = 0;
	do {
		hyb_cmd_te pending = hyb_cmd_none;
		switch ((int) cmd) {
		case hyb_cmd_start:
		case hyb_cmd_stop:
		case hyb_cmd_restart:

			if (!hyb->run && !hyb->force_run)
				break;

			if (hyb->operation != hyb_cmd_none) {
				if (hyb->operation != cmd)
					pending = cmd;
				break;
			}
			hyb->operation = cmd;
			switch ((int) cmd) {
			case hyb_cmd_start:
				if (hyb->msg == hyb_msg_end || hyb->msg == hyb_msg_exit) {
					hyb->msg = hyb_msg_null;
				} else if (hyb->msg == hyb_msg_suspend
						|| hyb->msg == hyb_msg_wait) {
					hyb->msg = hyb_msg_yield;
				}
				break;
			case hyb_cmd_stop:
				if (hyb->msg == hyb_msg_null)
					hyb->msg = hyb_msg_exit;
				else if (hyb->msg == hyb_msg_yield || hyb->msg == hyb_msg_wait
						|| hyb->msg == hyb_msg_delayed)
					hyb->msg = hyb_msg_suspend;
				break;
			case hyb_cmd_restart:
				if (hyb->msg == hyb_msg_yield || hyb->msg == hyb_msg_wait
						|| hyb->msg == hyb_msg_delayed
						|| hyb->msg == hyb_msg_suspend
						|| hyb->msg == hyb_msg_end) {
					hyb->msg = hyb_msg_end;
					hyb->app_cb(hyb);
				}
				hyb->msg = hyb_msg_null;
				break;
			}
			hyb->operation = hyb_cmd_none;
			break;

		case hyb_cmd_run:
		case hyb_cmd_force_run:

			if (hyb->app_cb
					&& ((hyb->run && cmd == hyb_cmd_run)
							|| (hyb->force_run && cmd == hyb_cmd_force_run))) {
				if (hyb->operation != hyb_cmd_none) {
					if (hyb->operation != cmd)
						pending = cmd;
					break;
				}
				hyb->operation = cmd;
				if (hyb->msg == hyb_msg_delayed || hyb->msg == hyb_msg_wait) {
					long diff = hyb->tbase - hyb_get_tick();
					if (diff <= 0) {
						hyb->msg = hyb_msg_yield;
					}
				}

				if (hyb->msg == hyb_msg_null)
					hyb->msg = hyb_msg_init;
				else if (hyb->msg == hyb_msg_yield)
					hyb->msg = hyb_msg_busy;

				if (hyb->msg == hyb_msg_init || hyb->msg == hyb_msg_busy) {
					hyb->app_cb(hyb);
					if (hyb->msg == hyb_msg_init)
						hyb->msg = hyb_msg_exit;
					else if (hyb->msg == hyb_msg_busy)
						hyb->msg = hyb_msg_end;
					else if (hyb->msg == hyb_msg_yield) {
						hyb->tbase = hyb_get_tick();
					}
				}
				if (hyb->msg == hyb_msg_yield || hyb->msg == hyb_msg_wait
						|| hyb->msg == hyb_msg_delayed)
					if (tmin && *tmin > hyb->tbase)
						*tmin = hyb->tbase;
				hyb->operation = hyb_cmd_none;
			}

			break;
		case hyb_cmd_free:
			if (hyb->operation != hyb_cmd_none) {
				if (hyb->operation != cmd) {
					pending = cmd;
					hyb->run = false;
					hyb->force_run = false;
				}
				break;
			}
			if (hyb->totalerr & hyb_ret_pending & hyb_ret_deadlock) {
				pending = cmd;
				hyb->run = false;
				hyb->force_run = false;
				break;
			}

			hyb->operation = cmd;
			if (hyb->msg == hyb_msg_yield || hyb->msg == hyb_msg_wait
					|| hyb->msg == hyb_msg_delayed
					|| hyb->msg == hyb_msg_suspend || hyb->msg == hyb_msg_end) {
				hyb->msg = hyb_msg_end;
				hyb->app_cb(hyb);
			}

			qnode_ts *qn = queue_peek_node(&hyb->relations);
			while (qn != 0) {
				hyb_relation_ts *hn = qnode_get_obj(qn);
				qn = qnode_peek_next(qn);
				hyb_relation_break(hn);
			}

			if (hyb->is_dyn) {
				_memory_free(hyb);
			}
			break;
		}

		if (cmd == hyb_cmd_free && pending == hyb_cmd_none) {
			return hyb_ret_free;
		}
		if (hyb->pending != hyb_cmd_none) {
			if (pending_cnt > 1) {
				return hyb_ret_deadlock;
			}
			cmd = hyb->pending;
			hyb->pending = hyb_cmd_none;
			pending_cnt++;
			continue;
		}

		hyb->pending = pending;
		if (hyb->pending != hyb_cmd_none)
			return hyb_ret_pending;
		break;
	} while (1);

	return hyb_ret_ok;
}
//\\\_\__\___\____\_____\______
//\\\			RULES FUNCTIONS
//\\\_\__\___\____\_____\______
void hyb_set_runnability(hyb_ts *hr, bool force_run, bool run) {
	hr->force_run = force_run;
	hr->run = run;
}
void hyb_set_rule_link(hyb_relation_ts *hr, bool up, bool down) {
	hr->up_link = up;
	hr->down_link = down;
}
void hyb_set_rule_force_run(hyb_relation_ts *hr, bool up, bool down) {
	hr->up_force_run = up;
	hr->down_force_run = down;
}
void hyb_set_rule_run(hyb_relation_ts *hr, bool up, bool down) {
	hr->up_run = up;
	hr->down_run = down;
}
void hyb_set_rule_free(hyb_relation_ts *hr, bool up, bool down) {
	hr->up_free = up;
	hr->down_free = down;
}

//\\\_\__\___\____\_____\______
//\\\			COMMAND FUNCTIONS
//\\\_\__\___\____\_____\______

hyb_ret_te hyb_start(hyb_ts *hyb, void *sgn_param) {
	if (!hyb)
		return hyb_ret_error;
	hyb->sgn_param = sgn_param;

	return _hyb_command(hyb, 0, hyb_cmd_start);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

hyb_ret_te hyb_stop(hyb_ts *hyb) {
	return _hyb_command(hyb, 0, hyb_cmd_stop);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
hyb_ret_te hyb_restart(hyb_ts *hyb) {
	return _hyb_command(hyb, 0, hyb_cmd_restart);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
hyb_ret_te hyb_force_run(hyb_ts *hyb, void *sgn_param) {
	if (!hyb)
		return hyb_ret_error;
	hyb->sgn_param = sgn_param;
	return _hyb_command(hyb, 0, hyb_cmd_force_run);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
hyb_ret_te hyb_run(hyb_ts *hyb, long *tick_min) {
	return _hyb_command(hyb, tick_min, hyb_cmd_run);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
hyb_ret_te hyb_free(hyb_ts *hyb) {
	return _hyb_command(hyb, 0, hyb_cmd_free);
}
//\\\_\__\___\____\_____\______
//\\\			NOTIFY FUNCTIONS
//\\\_\__\___\____\_____\______
hyb_ret_te hyb_notify(hyb_ts *hyb, void *sgn_param) {
	if (!hyb)
		return hyb_ret_error;
	hyb->sgn_param = sgn_param;
	_hyb_command(hyb, 0, hyb_cmd_start);
	return _hyb_command(hyb, 0, hyb_cmd_force_run);
}
//\\\_\__\___\____\_____\______
//\\\			RAW FUNCTIONS
//\\\_\__\___\____\_____\______
/* relation find ozelligi gelebilir*/
void hyb_relation_break(hyb_relation_ts *hn) {
	qnode_release(&hn->qn_up);
	qnode_release(&hn->qn_down);
	if (hn->is_dyn)
		_memory_free(hn);
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
hyb_ret_te hyb_relation_add(hyb_relation_ts *hn, hyb_ts *up, hyb_ts *down) {
	if (!hn || !up || !down)
		return hyb_ret_error;
	nullset(hn, hyb_relation_ts);

	qnode_set_obj(&hn->qn_up, hn);
	qnode_set_obj(&hn->qn_down, hn);
	hn->up = up;
	hn->down = down;

	qnode_set_prio(&hn->qn_up, up->prio);
	qnode_set_mostprio(&hn->qn_up, true);
	queue_send_node(&up->relations, &hn->qn_up);

	qnode_set_prio(&hn->qn_down, down->prio);
	qnode_set_mostprio(&hn->qn_down, true);
	queue_send_node(&down->relations, &hn->qn_down);

	return hyb_ret_ok;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
hyb_relation_ts* hyb_relation_create(hyb_ts *up, hyb_ts *down) {
	if (!up || !down)
		return 0;
	hyb_relation_ts *hn = _memory_allocation(sizeof(hyb_relation_ts));
	hyb_ret_te ret = hyb_relation_add(hn, up, down);
	if (ret != hyb_ret_ok)
		return 0;
	hn->is_dyn = 1;
	return hn;
}

hyb_ret_te hyb_init(hyb_ts *hyb, void *func, void *param, unsigned long prio) {
	if (!hyb)
		return hyb_ret_error;
	nullset(hyb, hyb_ts);
	hyb->app_cb = func;
	hyb->app_param = param;

	hyb->prio = prio;

	return hyb_ret_ok;
}

hyb_ts* hyb_create(void *func, void *param, long prio) {
	hyb_ts *hyb = _memory_allocation(sizeof(hyb_ts));
	if (!hyb)
		return 0;
	hyb_ret_te ret = hyb_init(hyb, func, param, prio);
	if (ret != hyb_ret_ok) {
		_memory_free(hyb);
		return 0;
	}

	hyb->is_dyn = 1;

	return hyb;
}
//\\\_\__\___\____\_____\______
//\\\			THREAD FUNCTIONS
//\\\_\__\___\____\_____\______

hyb_relation_ts* hyb_thread_add(hyb_ts *parent, hyb_ts *hyb) {
	if (!parent || !hyb)
		return 0;

	hyb_relation_ts *hr = hyb_relation_create(parent, hyb);
	if (!hr) {
		return 0;
	}

	hyb_set_rule_link(hr, true, false);
	hyb_set_rule_force_run(hr, false, true);
	hyb_set_rule_run(hr, true, false);
	hyb_set_rule_free(hr, true, false);

	hyb_force_run(parent, 0);
	return hr;
}

hyb_ts* hyb_thread_create(hyb_ts *parent, void *func, void *param, long prio) {
	hyb_ts *hyb = hyb_create(func, param, prio);
	if (!hyb)
		return 0;

	hyb_set_runnability(hyb, false, true);
	if (parent) {
		if (!hyb_thread_add(parent, hyb)) {
			_memory_free(hyb);
			return 0;
		}
	}

	return hyb;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
typedef struct {
	queue_ts q;
	queue_size_t limit;
} _hyb_queue_ts;

static void _hyb_queue_cb(hyb_ts *hyb) {
	hyb_begin
	;
	queue_size_t *limit = hyb_get_param();
	_hyb_queue_ts *p = _memory_allocation(sizeof(_hyb_queue_ts));
	hyb_get_param() = p;
	nullset(p, _hyb_queue_ts);
	p->limit = *limit;
	hyb_end
;
_memory_free(hyb_get_param());
}

hyb_relation_ts* hyb_queue_add(hyb_ts *parent, hyb_ts *hyb) {
if (!parent || !hyb)
	return 0;

hyb_relation_ts *hr = hyb_relation_create(parent, hyb);
if (!hr) {
	return 0;
}

hyb_set_rule_free(hr, true, false);
return hr;
}

hyb_ts* hyb_queue_create(hyb_ts *parent, queue_size_t limit) {
hyb_ts *hyb = hyb_create(_hyb_queue_cb, &limit, 0);
if (!hyb)
	return 0;

hyb_set_runnability(hyb, false, true);
hyb_run(hyb, 0);

if (parent) {
	if (!hyb_queue_add(parent, hyb)) {
		_memory_free(hyb);
		return 0;
	}
}

return hyb;
}
/* Fonksiyon hyb_queue icin wait ediyorken time out olursa ilgili relation da otomatik kopmali*/
static hyb_ret_te _hyb_queue_command(hyb_ts *hyb, long *sgn_param,
	qnode_prio_t prio, bool is_send) {
hyb_ret_te ret = hyb_ret_ok;
_hyb_queue_ts *hq = hyb_get_param();
hyb_ts *sub = 0;
hyb_relation_ts *hr = 0;
qnode_ts *qn = queue_peek_node(&hyb->relations);
while (qn != 0) {

	hr = qnode_get_obj(qn);

	bool isdown = false;
	if (qn == &hr->qn_up) {
		isdown = true;
		sub = hr->down;
	} else if (qn == &hr->qn_down) {
		isdown = false;
		sub = hr->up;
	}

	qn = qnode_peek_next(qn);
	if (!sub)
		continue;
	if (!isdown) {
		sub = 0;
	}
}

if (is_send) {
	/*SEND*/
	if (sub) {
		/*hazir alici bekliyor, dogrudan ilgili aliciya yonlendirilir*/
		long *value = 0;
		if (queue_get_size(&hq->q) > 0) {
			queue_send_obj(&hq->q, sgn_param, prio, 0);
			value = queue_recv_obj(&hq->q);
		} else {
			value = sgn_param;
		}

		hyb_relation_break(hr);
		hyb_notify(sub, (void*) value);
	} else {

		if (hq->limit == 0 || queue_get_size(&hq->q) < hq->limit) {
			/* kuyruga alinir*/
			queue_send_obj(&hq->q, sgn_param, prio, 0);
		} else {
			/* kuyruk dolu bildirimi yapilir ve caller ile iliski kurulur*/
			ret = hyb_ret_pending;
		}
	}
} else {
	/*RECEIVE*/

	if (sub) {
		/* gonderici bekletiliyor*/
		queue_send_obj(&hq->q, (long*) sub->sgn_param, prio, 0);
	}

	if (queue_get_size(&hq->q) > 0) {
		long *value = 0;
		value = queue_recv_obj(&hq->q);
		*sgn_param = (long) value;
		if (sub) {
			/* gonderici bekletiliyor*/
			hyb_relation_break(hr);
			hyb_notify(sub, value);
		}
	} else {
		ret = hyb_ret_pending;
	}
}

return ret;
}

hyb_ret_te hyb_queue_send(hyb_ts *hyb, void *sgn_param, qnode_prio_t prio) {
return _hyb_queue_command(hyb, sgn_param, prio, true);
}

hyb_ret_te hyb_queue_recv(hyb_ts *hyb, void *sgn_param) {
return _hyb_queue_command(hyb, sgn_param, 0, false);
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
