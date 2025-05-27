
/*****************************************************************
  * @File					:	hybos_single.c
  * @Author				:	Hasan KARA
  * @Revision				:	1
  * @Date					:	14.03.2021 CUMA, 19:43
  * @Brief					:	hybos thread system port use Cooperative
  * @Attention			:
  *
  * <h2><center>&copy; COPYRIGHT(c) hasankara.info</center></h2>
  ***************************************************************/

//\\\_\__\___\____\_____\______
//\\\			INCLUDES
//\\\_\__\___\____\_____\______
#include "hybos_threads.h"
#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include "stdbool.h"

//\\\_\__\___\____\_____\______
//\\\			VARIABLES
//\\\_\__\___\____\_____\______
static bool isNotified = false;

//\\\_\__\___\____\_____\______
//\\\			STATIC FUNCTIONS
//\\\_\__\___\____\_____\______
static long critical_cnt;
static void _enter_critical_fnc(void)
{
	if(critical_cnt < 0xffff) critical_cnt++;
	if(critical_cnt == 1)__disable_irq();

}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
static void _exit_critical_fnc(void)
{
	if(critical_cnt > 0) critical_cnt--;
	if(critical_cnt == 0)__enable_irq();

}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

static void notified_cb(hyb_ts *hyb)
{
	hyb_begin;
	while (1){
		hyb_yield();
		isNotified = true;
	}
	hyb_end;
}

//\\\_\__\___\____\_____\______
//\\\			GLOBAL FUNCTIONS
//\\\_\__\___\____\_____\______
hyb_ts *hyb_cooperative_create(void)
{
	hyb_ts *hyb = hyb_create(&notified_cb, 0, 0);
	hyb_set_runnability(hyb, true, false);
	return hyb;
}

hyb_ret_te hyb_cooperative_handler(hyb_ts *hyb)
{
	hyb_ret_te ret= hyb_ret_error;
	critical_cnt = 0;
	while (1)
	{

		long tick_min = HYB_MAX_DELAY ;
		ret = hyb_run(hyb, &tick_min);
		if (ret == hyb_ret_ok)
		{
			while(tick_min > hyb_get_tick() && isNotified==false)
			{
				/*
				The RTC alarm is set for the tick_min duration and the CPU goes into sleep mode.
				*/
			}
			isNotified = false;
		}
		else if (ret == hyb_ret_free)
		{
			break;
		}
	}
	return ret;
}


void hyb_init_port()
{

	queue_cb_init_critical(&_enter_critical_fnc, &_exit_critical_fnc);
	queue_cb_init_dynamic_memory(&malloc, &free);
	hyb_get_tick = (void *)&Delay_Peek_Us;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
