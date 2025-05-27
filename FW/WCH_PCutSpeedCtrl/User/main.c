/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : hasankarainfo
 * Version            : V_01b
 * Date               : 2025/04/15
 * Description        : WCH_PCutSpeedCTRL main program body. 
 *	It ensures that the motor, suitable for operation with 220 vac 50hz, operates at constant speed.
 *	It was developed using the ch32v003j4m6 microcontroller from wch company.
 *	Some benefits of the firmware;
 *	Suitable for use with standard hand milling motors that are easily available on the world market.
 *	The mentioned milling motors are suitable for use in CNC machines,
 *	Any CNC controller can be connected directly and in isolation with a standard PWM output.
 *********************************************************************************
 * Copyright (c) hasankarainfo
 *******************************************************************************/

#include "init_wch.h"
#include "debug.h"
#include <stdbool.h>
#include "diff_int.h"
#include "NTC_Table.h"

#include "hybos_threads.h"

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#define DISABLE		0
#define ENABLE 		!DISABLE

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*FIRMWARE MANAGEMENT*/
/* If set ENABLE:	Operates at CONSTANT SPEED */
/* If set DISABLE:	Operates at CONSTANT DUTY */
#define run_mode_closed_loop_en			ENABLE
/* If set ENABLE:	Duty values are passed through the linearization power transfer function */
/* If set DISABLE:	Linearization power transfer function is skipped */
#define run_mode_linearization			ENABLE

/* If set linFuncMode_Equation: Use Sine power table and math equation*/
#define linFuncMode_Equation			0
/* If set linFuncMode_Measured: Use self prepaired power transmission table*/
#define linFuncMode_Measured			1
#define linFuncMode						linFuncMode_Equation

/*	Some DEBUG values puts with over the serial port. */
/* All debug lines and uart inital lines can be disable*/
#define debug_en						DISABLE
/* Boot up puts, measured power transmission table inverse scale function applied values */
#define debug_sysLin_Puts_en			ENABLE
/* Periodic puts, input capture values*/
#define debug_IC_en						DISABLE
/* Periodic puts, "target rpm, actual rpm, duty" values in line*/
#define debug_set_act_duty_en			ENABLE
/* Periodic puts, "proportional, Integrate, Derivative" values in line*/
#define debug_PID_en					ENABLE

/*	if set ENABLE:	activate WCH microcontroller ADC hardware init routines
 *		In the future of the project, features such as speed control with 
 *		potentiometer or temperature protection with NTC may be needed. */
#define ADC_mode 						DISABLE

/* Periodic some debug puts periode value, unit mili second */
const int dbg_per	=	300;
/* Timing task periode value, unit mili second */
const int timing_task_per	=	5;
/* wait for delay time for boot up duty operations unit mili second */
const int StartDly	=	5;
/* The minimum target speed value that must be entered to start working unit rpm */
const int MinTgt 	=	1000;
/* The maximum target speed value unit rpm */
const int MaxTgt 	=	30000;
/* Difference between pwm spin up and stop threshold values  */
const int PwmHysteresis =	4; 
/* Lowest pwm value */
const int PwmLow 	=	31;
/* Highest pwm value */
const int PwmHigh 	=	1005;

const int measScaleMult 	= 10000;

/* The PWM value taken with input capture is not directly equalized to the target speed value.
*  Each time a zero cross is detected, the target speed and the entered speed value get closer step by step.
*  For hardware operating at 50Hz line frequency, a step value is added or subtracted every 10 milliseconds.
*  Higher value is fast settling unit rpm */
const int TgtStep 	=	25;
/* The minimum value required to start the process */
const int MinDuty 	=	400; 
/* Maximum Duty value */
const int MaxDuty 	=	measScaleMult ;

/* PID Loop time is equal zerocross detection periode */
#define	k_p				6	/10
#define	k_i				7	/10
#define	k_d				300	/10
#define	k_d_filt		20
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*EXTERNS*/
extern hyb_ts *hyb_cooperative_create(void);
extern hyb_ret_te hyb_cooperative_handler(hyb_ts *hyb);
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
const int Periode =	10000; /* zero cross (zc) periode cycle time. unit micro second (us)*/
#define k_rpm				(60*1000)

#if (linFuncMode == linFuncMode_Equation)

/* The table is created With Octave PC Application */
const uint16_t inverseSinePowerAmplitude = 10000;
const uint16_t inverseSinePowerLength = 101;
const uint16_t inverseSinePowerTable[101] = {
   0,  638,  903, 1108, 1282, 1436, 1575, 1705, 1826, 1940,
2048, 2152, 2252, 2348, 2441, 2532, 2620, 2706, 2789, 2871,
2952, 3031, 3108, 3184, 3259, 3333, 3406, 3478, 3550, 3620,
3690, 3759, 3828, 3896, 3963, 4030, 4097, 4163, 4229, 4294,
4359, 4424, 4489, 4553, 4617, 4681, 4745, 4809, 4873, 4936,
5000, 5064, 5127, 5191, 5255, 5319, 5383, 5447, 5511, 5576,
5641, 5706, 5771, 5837, 5903, 5970, 6037, 6104, 6172, 6241,
6310, 6380, 6450, 6522, 6594, 6667, 6741, 6816, 6892, 6969,
7048, 7129, 7211, 7294, 7380, 7468, 7559, 7652, 7748, 7848,
7952, 8060, 8174, 8295, 8425, 8564, 8718, 8892, 9097, 9362,
10000};

#elif (linFuncMode == linFuncMode_Measured)
/*	Attention Please;
	The minimum value of all values in the table must be at index [0]
	The maximum value of all values in the table must be at index [Length-1]

	Duty is increased at equal intervals and the feedback value is recorded.
	When performing this operation, "run_mode_closed_loop_en DISABLE", must be used.
	When performing this operation, "run_mode_linearization DISABLE", must be used.
*/
/*	Duty - Power Table	 */
const uint16_t measTable[] 	= {75,121,202,321,452,555,655,716,755,762,765};
const int measTable_Length 	= (sizeof(measTable)/2);
#endif
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

static long limit(long x, long min, long max) {
	long val = x;
	if (max < min) {
		if (val > min)
			val = min;
		if (val < max)
			val = max;
	} else {
		if (val > max)
			val = max;
		if (val < min)
			val = min;
	}
	return val;
}
static long map(long x, long in_min, long in_max, long out_min, long out_max) {
	long val = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	val = limit(val, out_min, out_max);

	return val;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*SOME GLOBAL VARIABLES*/

int Summ = 0;
int prevError = 0;
int Portional = 0;
int Deriv = 0;
int Duty = 0;
int error = 0;
int tgt_rpm;
int pwm_in;
int act_rpm;
int act_ms;
int calc_us;

uint8_t	f_cnt1 = 0;
uint8_t	f_cnt2 = 0;

hyb_ts	*hyb_globes, *hyb_zc_detect, *hyb_triac_sgn;
diff_ts d_zc;
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/*VARIABLES and FUNCTIONS for ADC*/
#if (ADC_mode == ENABLE)

typedef enum {
	arg_throttle,
	arg_periode,
	//arg_min, /**/
	//arg_offset,
	arg_size
} arg_enumeration;
//static uint8_t adc_chs[]={ADC_Channel_2, ADC_Channel_0 , ADC_Channel_3, ADC_Channel_1};
// static uint8_t adc_chs[]={ADC_Channel_2, ADC_Channel_0 };
static uint8_t adc_chs[]={ADC_Channel_6, ADC_Channel_7 };
static uint8_t adc_nbrs = arg_size;

static diff_ts d_adc[arg_size];
long arg[arg_size];
long adc_val[arg_size];
long per_raw;

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void ADC_Result_Rdy(void) {
	//	temp = NTC_ADC2Temperature(ntc);

	//	arg[arg_periode] = map(diff_peek(&d_adc[arg_periode]) , 25,1010,0 , 12000);
	//	arg[arg_min] = map(diff_peek(&d_adc[arg_min]) , 0,1020, 0 , arg[arg_periode]/2);
	//	arg[arg_throttle] = map(diff_peek(&d_adc[arg_throttle]), 25, 1010, 0, arg[arg_periode]- 500);
	//	arg[arg_throttle] = map(diff_peek(&d_adc[arg_throttle]), 25, 1010, 0, 100);
	//	PWM_SET(arg[arg_throttle]/5);
}
#endif
///\\\-\---\----\------\-------\--------\----------\-----------\-------------


#if (linFuncMode == linFuncMode_Equation)

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
int inverseScaleEquation(int in)
{
	int step =0;
	int y1;
	int tmp = 0;
	int val = in;
	
	step = measScaleMult/2;
	y1 = step;

	while(  1 )
	{
		step/=2;
		//tmp = sinus_power_equation(y1);
		if(val < tmp)
		{
			y1-=step;
		}else if(val > tmp){
			y1+=step;
		} else{
			/* exact found*/
			break;
		}
		if(step<1)
		{
			break;
		}
		
	}
		
	return y1;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
int interpolateTable(int in, const uint16_t *ptr, int len, int Amp)
{
	/* Example; tmp = 250 => ind = 2, valn = 1108, val= 903,  result = 1005 */
	in &=0xefffffff;
	len &=0xefffffff;
	int ind =  in / (len-1);
	if(ind >= (len-1)  ) 
	{
		return ptr[len-1];
	}
	//								step*ind	tmp		step*(ind+1)
	/* result = val + delta_y / (	  200		250		  300)			*/
	int val = ptr[ind];
	int valn = ptr[ind+1];
	//result = val + (valn-val) * (tmp - ind*step )/ step;
	return val + (valn-val) * (in*(len-1) - ind*Amp )/ Amp;
}
#elif (linFuncMode == linFuncMode_Measured)
/*********************************************************************
 * @fn      inverseInterpolateTable
 *
 * @brief   The lookup table obtained with the unit impulse response of the system is processed.
 *			The table is scaled and inverse is applied.
 *			Two points are detected and linear interpolation is applied.
 *
 * @param 	in - entering value
 *			ptr - Table Pointer
 *			len - Table Length
 *			scale - scale factor value
 *
 * @return  Resulting value
 */
int inverseInterpolateTable(int in, const uint16_t *ptr, int len, int scale)
{
	int f_val = 0;
	int l_val = 0;
	int width = 0;
	int val=0, valn=0;
	int j=0;
	int delta_x = 0;
	int delta_y = 0;

	f_val = ptr[0];
	//f_val = 0;
	l_val = ptr[len - 1];
	width = l_val - f_val;

	while(  j < len )
	{
		val = (ptr[j] - f_val)* scale /width ;//1087
		j++;
		valn= (ptr[j] - f_val) * scale /width ;//1753

		/* search index*/
		if(val <= in && in <= valn)
		{
			break;
		}
	}

	/* example => valn = 5000, val = 1000, in = 4000, j = 5  */
	delta_y = (valn-val); // delta_y = 4000
	/* delta_x = ((j+1) -j) * scale/len;	*/
	delta_x = scale / len; // delta_x = 909

	/*
	val 	-> in 		3 step
	in 		-> valn 	1 step
	
	fraction = (delta_x)	* ( (in-val) 	/	delta_y 	)
	fraction = (delta_x) 	* (	(3 step) 	/ 	(4 step) 	)
	fraction = 	 909 		* 		3 		/ 		4 			= 682

	result = j * delta_x + fraction
	result = j * delta_x + (delta_x)* ( (in-val) 	/	delta_y 	)
	result = j * delta_x + (delta_x)* ( (in-val) 	/	(valn-val) 	)
				5	  909		 909		 3000 		/ 	  4000
	result = 5 * 909 + 682 = 5227
	*/

	if(delta_y == 0) return 0;
	//return  j * delta_x +  (in - val) * delta_x  / delta_y; 

	// for more fraction
	return  j * scale / len +  (in - val) * scale / len / delta_y; 
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
#endif
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

int sys_linearization(int numerator)
{
	/* system linearisation with the sinus function integral and power equation targeting*/
	/* "https://www.desmos.com/calculator/x6jm10zomc" link available to see the chart*/
	int tmp = numerator;
#if (linFuncMode == linFuncMode_Equation)
	// tmp = map(tmp,MinDuty,measScaleMult, MinDuty, MaxDuty);

	tmp = interpolateTable(tmp, inverseSinePowerTable, inverseSinePowerLength, inverseSinePowerAmplitude);

#elif (linFuncMode == linFuncMode_Measured)
	
	tmp = inverseInterpolateTable(tmp, measTable, measTable_Length, measScaleMult);
#endif

	return tmp;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

void task_zc_detect(hyb_ts *hyb){
	hyb_begin;

	while (1)
	{
		if(hyb_get_sgnparam() == 0)
			hyb_suspend();

		hyb_get_sgnparam() = 0;

		if(tgt_rpm   < MinTgt){
			Duty = 0;
			Summ = 0;
			error = tgt_rpm - act_rpm;
			prevError = error;
			continue;
		}
	
#if (run_mode_closed_loop_en == ENABLE)
		int tmp = 0;
		int k_sens_reaction = limit(act_ms,10,300);
		error = tgt_rpm - act_rpm;

		/* P */
		Portional = error  * k_p ;
		Portional = limit(Portional, -measScaleMult/2, measScaleMult/2 );

		/* I */
		tmp = error  * k_i  / k_sens_reaction;
		Summ += tmp;
		Summ = limit(Summ, 200, measScaleMult);

		/* D */
		tmp = (error - prevError) * k_d / k_sens_reaction;
		//tmp = (error - prevError) * k_d ;
		Deriv = (tmp + Deriv*k_d_filt)/(k_d_filt+1);/* filter apply*/
		Deriv = limit(Deriv, -measScaleMult/2, measScaleMult/2 );
		
		/* TOTAL */
		Duty = Portional + Summ + Deriv ;
		prevError = error;
#else 
		/* tgt_rpm values must be remap for duty*/
		if(tgt_rpm >= MinTgt)
			Duty = map(tgt_rpm, MinTgt, MaxTgt, 0, MaxDuty);
		else Duty = 0;

#endif
	Duty = limit(Duty, 0, measScaleMult );

#if (run_mode_linearization == ENABLE)
		Duty = sys_linearization(Duty);
#endif
		
		calc_us = diff_get(&d_zc);
		if(Duty >= MinDuty )
		{
			int time = Periode - Duty - calc_us;
			hyb_wait(time);
			/* If the zc signal occurs during this time, the currently running cycle is skipped.*/
			if(hyb_get_sgnparam() == 0)
			{
				hyb_notify(hyb_triac_sgn,  0);
			}
			
		}else {
			Duty = 0;
		}
		
	}
	hyb_end;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void zc_detected(void){
	//if(isZc()!=0)return;
	diff_reset(&d_zc);
	hyb_notify(hyb_zc_detect,  (int*)!0);
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void task_triac_sgn(hyb_ts *hyb){
	hyb_begin;

	while (1)
	{
		hyb_suspend();
		Triac(1);
		hyb_delay(500+Duty/4);
		
		Triac(0);
	}
	hyb_end;
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* PWM Input*/
void IC1_det(uint16_t ris, uint16_t fal)
{
	static int estimate;
	
	int tmp = fal* (PwmHigh + 5) / ris;
	estimate = (tmp + estimate * 9)/10;

	if(estimate*15/10 > tmp &&
	estimate*5/10 < tmp)
	{
		f_cnt1 = 0;
		pwm_in = tmp;
	}
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* Speed Measure*/
void IC2_det(uint16_t ris, uint16_t fal)
{
	static int estimate;
	
	int tmp = fal;
	estimate = (tmp + estimate * 9)/10;

	if(estimate*15/10 > tmp &&
	estimate*5/10 < tmp)
	{
		f_cnt2 = 0;
		act_ms = tmp/255;
		act_rpm = ((int)k_rpm*255/fal);
	}
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void ramp_tgt_rpm()
{
	static char before_state ;
	int tmp = pwm_in;

	if(tmp   < (PwmLow - before_state * PwmHysteresis)){
		before_state = 0;
		if(tgt_rpm > TgtStep)	tgt_rpm -= TgtStep;
		else {
			tgt_rpm = 0;
		}
		
		
	}
	else {
		before_state = 1;
		tmp = map(tmp, PwmLow, PwmHigh, MinTgt, MaxTgt);
		if		(tmp > tgt_rpm + TgtStep)	tgt_rpm += TgtStep;
		else if	(tmp + TgtStep < tgt_rpm )	tgt_rpm -= TgtStep;
		else 								tgt_rpm = tmp;
	}
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* Some values Timeout and Ramp handler Task Callback Function */
void task_timing_handler(hyb_ts *hyb){
	hyb_begin;

#if (debug_en == ENABLE)
	USART_Printf_Init(hyb_globes, 115200);
	debug_printf("init %s %s  \r\n",__DATE__, __TIME__);
	debug_printf("SystemClk:%d\r\n",SystemCoreClock);
	debug_printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	
	hyb_thread_create(hyb_globes, &task_debug,0,1);
#endif
#if (ADC_mode == ENABLE)
	for (int i = 0; i < adc_nbrs; i++) {
		diff_init_def(&d_adc[i], &adc_val[i]);
		diff_en_rev_tresh(&d_adc[i], 40);
	}

	/*ADC INIT*/
	ADC_HW_Init(&ADC_Result_Rdy , adc_chs, adc_nbrs, adc_val);
#endif
	/* Init Triac Out */
	Init_TriacOut();
	/* Init IC1 for PWM Input*/
	Input_Capture_Init(0xffff, 48-1, &IC1_det);
	/* Init IC2 for Speed Measure*/
	Input_Capture_Init2(0xffff, 192-1, &IC2_det);

	diff_init_def(&d_zc, &sys_tick_us);

	/* Init External Interrupt for Zero Cross Detection*/
	EXTI_app_Init( &zc_detected );

	hyb_delay(StartDly*1000);

	hyb_zc_detect = hyb_thread_create(hyb_globes, &task_zc_detect, 0, 5);
	hyb_triac_sgn = hyb_thread_create(hyb_globes, &task_triac_sgn, 0, 4);
	 while (1){
		hyb_delay(timing_task_per*1000);
		/* IC1 Command PWM Input*/
		int tmp = f_cnt1 * timing_task_per;
		if(tmp > 300)
		{
			if(isPwm()){

				pwm_in = MaxTgt;
			}else{
				pwm_in = 0;
				Summ = 0;
				prevError = 0;
				Duty = 0;
			}
			
		}else{
			f_cnt1 ++;
		}
		ramp_tgt_rpm();

		/* IC2 Speed Measure*/
		tmp = f_cnt2 * timing_task_per;
		if(tmp > 300)
		{
			act_ms = 300;
			act_rpm = k_rpm/act_ms;
			
		}else{
			if(tmp > act_ms)
			{
				act_ms = tmp;
				act_rpm = k_rpm/act_ms;
			}
			f_cnt2 ++;
		}
	 }
	hyb_end;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

/* Debug Task Callback Function*/
#if (debug_en == ENABLE)
void task_debug(hyb_ts *hyb){
	hyb_begin;
static int i=0, j=0 , step=0;

#if(debug_sysLin_Puts_en == ENABLE)
	step = 100;
	debug_printf("\ninverseSinePowerTable = ([");
	hyb_delay(25000);
	for(i=0 ; i<= measScaleMult ; i+=j)
	{
		debug_printf("...\n");
		for ( j = 0 ;j < step * 10; j += step){
			debug_printf("%04d" , sys_linearization(i+j));
			
			if((i+j) > measScaleMult)break;
			debug_printf(", ");
		}
		hyb_delay(40000);
	}
	debug_printf("]); \n");
#endif

	while (1){
		hyb_delay(dbg_per*1000);
		debug_printf("\n");
#if(debug_IC_en == ENABLE)
		
		debug_printf("R1_F1 %04d_%04d\n" ,  (int)IC_rising,  (int)IC_falling);
		debug_printf("R2_F2 %04d_%04d\n", (int)IC_rising2,  (int)IC_falling2);
#endif
#if(debug_set_act_duty_en == ENABLE)
		debug_printf("Set_ActRpm_Duty %04d_%04d_%04d\n" ,  tgt_rpm,  act_rpm, Duty);
		debug_printf("calc_us %04d\n" ,  calc_us);
#endif
#if(debug_PID_en == ENABLE)
		debug_printf("P_I_D %04d_%04d_%04d\n" , Portional,  (int)Summ, (int)Deriv);
#endif
	 }
	hyb_end;
}
#endif
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/***************************************************************************/
/********************************	MAIN	********************************/
/***************************************************************************/
int main(void) 
{
	Init_CPU();
	Delay_Init();
	hyb_init_port();
	hyb_globes = hyb_cooperative_create();

	hyb_thread_create(hyb_globes, &task_timing_handler,0,4);
	hyb_cooperative_handler(hyb_globes);
}
