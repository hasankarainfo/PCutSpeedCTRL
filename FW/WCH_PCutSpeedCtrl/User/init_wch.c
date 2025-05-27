/********************************** (C) COPYRIGHT *******************************
 * File Name          : init_wch.c
 * Author             : hasankarainfo
 * Version            : V_01b
 * Date               : 2025/04/15
 * Description        : init hardware for WCH_PCutSpeedCTRL projects.
*********************************************************************************
* Copyright (c) hasankarainfo
*******************************************************************************/

#include "init_wch.h"

/* Global define */
typedef enum {CMD_OK = 0, CMD_ERR = ~CMD_OK} CmdStatus;


static void (*_IC1_cb)(uint16_t ris, uint16_t fal  );
static void (*_IC2_cb)(uint16_t ris, uint16_t fal  );
static void (*_exti_cb)(void );

///\\\-\---\----\------\-------\--------\----------\-----------\-------------

void Init_CPU(void)
{
	NVIC_PriorityGroupConfig(select_NVIC_PriorityGroup);
	SystemCoreClockUpdate();
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------

void Init_TriacOut(void)
{
	/*Output Drive Triac*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure = { 0 };
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_InitStructure.GPIO_Pin = _pin_triac;
	GPIO_Init( _port_triac, &GPIO_InitStructure);
    Triac(0);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Input_Capture_Init(u16 arr, u16 psc, void (*cb)(uint16_t ris, uint16_t fal  ))
{
    _IC1_cb = cb;
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_ICInitTypeDef       TIM_ICInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef        NVIC_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;

#if (SelectMCU  == CH32V003F) // TSSOP20
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ITConfig(TIM1, TIM_IT_CC1 | TIM_IT_CC2  , ENABLE);
    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);

#elif (SelectMCU  == CH32V003J) // SO8
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1, ENABLE);

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO , ENABLE );
    

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ITConfig(TIM1, TIM_IT_CC1 | TIM_IT_CC2  , ENABLE);
    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
#endif
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV4;// default 1
    TIM_ICInitStructure.TIM_ICFilter = 0x03;// default 0
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;// default rising
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;// default direct ti
    TIM_PWMIConfig(TIM1, &TIM_ICInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = select_NVIC_PriorityGroup;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);

    TIM_Cmd(TIM1, ENABLE);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void TIM1_CC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_CC_IRQHandler(void)
{
    static uint16_t ris, fal;
    if( TIM_GetITStatus( TIM1, TIM_IT_CC1 ) != RESET )
    {
        ris = TIM_GetCapture1( TIM1 );
    }

    if( TIM_GetITStatus( TIM1, TIM_IT_CC2 ) != RESET )
    {
        fal = TIM_GetCapture2( TIM1 );
        if(_IC1_cb) _IC1_cb(ris, fal);
    }

    TIM_ClearITPendingBit( TIM1, TIM_IT_CC1 | TIM_IT_CC2 );
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Input_Capture_Init2(u16 arr, u16 psc, void (*cb)(uint16_t ris, uint16_t fal  ))
{
    _IC2_cb = cb;
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_ICInitTypeDef       TIM_ICInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef        NVIC_InitStructure = {0};

#if (SelectMCU  == CH32V003F) // TSSOP20
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_PWR, ENABLE );
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_1);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);

    TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2  , ENABLE);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);

#elif (SelectMCU  == CH32V003J) // SO8

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_PWR, ENABLE );
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_2);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);

    TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 , ENABLE);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);

#endif

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);


    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;// default 1
    TIM_ICInitStructure.TIM_ICFilter = 0x03;// default 0
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;// default rising
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;// default direct ti
    TIM_PWMIConfig(TIM2, &TIM_ICInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = select_NVIC_PriorityGroup;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);

    TIM_Cmd(TIM2, ENABLE);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)
{
    static uint16_t ris, fal;
    if( TIM_GetITStatus( TIM2, TIM_IT_CC1 ) != RESET )
    {
        ris = TIM_GetCapture1( TIM2 );
    }

    if( TIM_GetITStatus( TIM2, TIM_IT_CC2 ) != RESET )
    {
        fal = TIM_GetCapture2( TIM2 );
        if(_IC2_cb) _IC2_cb(ris, fal);
    }

    TIM_ClearITPendingBit( TIM2, TIM_IT_CC1 | TIM_IT_CC2  );
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------


void EXTI_app_Init(  void* cb)
{
	_exti_cb = cb;

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | selectRCC(_port_zc) , ENABLE);

    /*This block was taken from the wch example application. Its purpose is not understood.*/
    	asm volatile(
    #if __GNUC__ > 10
    		".option arch, +zicsr\n"
    #endif
    		"addi t1, x0, 3\n"
    		"csrrw x0, 0x804, t1\n"
    		 : : :  "t1" );

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = _pin_zc;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_Init( _port_zc, &GPIO_InitStructure);


    GPIO_EXTILineConfig(_exti_config_zc);
    EXTI_InitStructure.EXTI_Line = _line_zc;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = select_NVIC_PriorityGroup;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void)
{
	if(EXTI_GetITStatus(_line_zc)!=RESET)
	{
		if(_exti_cb) _exti_cb();
		EXTI_ClearITPendingBit(_line_zc);
	}

}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
/* ADC Definitions */

#define ADC_Samp_Cycle	ADC_SampleTime_57Cycles
/* must be equal or upper div16, otherwise lower values nonlinear*/
//#define ADC_Clk_Div		RCC_PCLK2_Div32
#define ADC_Clk_Div		RCC_PCLK2_Div64

/* ADC Variables*/
static uint8_t *adc_chs;
static uint8_t adc_nbrs;
static long	*adc_rslts;
static void (*adc_result_rdy)(void );
u8 adc_ind = 0;
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void ADC_HW_Init(void (*cb_rdy)(void ), uint8_t *_chs, uint8_t _nbrs, long *_rslts)
{

	adc_chs = _chs;
	adc_nbrs = _nbrs;
	adc_rslts = _rslts;
    adc_result_rdy = cb_rdy;
	/* every adc result_raw value has 10 bit. But we use High 8 bit.
	 * in this way, buffer size reduces half.
	 * Therefore, dma use peripheral addr "(u32)&ADC1->RDATAR + 1" */

    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(ADC_Clk_Div);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    // GPIO_Init(GPIOC, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
   GPIO_Init(GPIOD, &GPIO_InitStructure);


    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_75PERCENT);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    NVIC_InitTypeDef NVIC_InitStructure={0};
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = select_NVIC_PriorityGroup;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    ADC_ITConfig( ADC1, ADC_IT_EOC, ENABLE );

    ADC_RegularChannelConfig(ADC1, adc_chs[0], 1, ADC_Samp_Cycle);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
u16 Get_ADC_Val(u8 ch)
{
    u16 val;

    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_Samp_Cycle);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    val = ADC_GetConversionValue(ADC1);

    return val;
}

void ADC1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      ADC1_IRQHandler
 *
 * @brief   This function handles ADC1 Handler.
 *
 * @return  none
 */


void ADC1_IRQHandler(void)
{
	if(ADC_GetITStatus(ADC1, ADC_IT_EOC)!=RESET)
	{
		adc_rslts[adc_ind] = ADC_GetConversionValue(ADC1);

		if(++adc_ind >= adc_nbrs) adc_ind=0;

	    ADC_RegularChannelConfig(ADC1, adc_chs[adc_ind], 1, ADC_Samp_Cycle);
	    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	    if(adc_ind == 0 && adc_result_rdy!=0 ) adc_result_rdy();

		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);     /* Clear Flag */
	}
}



