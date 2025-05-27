/********************************** (C) COPYRIGHT *******************************
 * File Name          : init_wch.h
 * Author             : hasankarainfo
 * Version            : V_01b
 * Date               : 2025/04/15
 * Description        : init hardware for WCH_PCutSpeedCTRL projects.
 *********************************************************************************
 * Copyright (c) hasankarainfo
 *******************************************************************************/

#ifndef USER_INIT_PCUT_WCH_H_
#define USER_INIT_PCUT_WCH_H_

#include <ch32v00x_misc.h>
#include "stdbool.h"


#define CH32V003F  0 // TSSOP20
#define CH32V003J  1 // SO8
#define SelectMCU   CH32V003J

/* UART Printf Definition */
#define DEBUG_UART1_NoRemap   1  //Tx-PD5
#define DEBUG_UART1_Remap1    2  //Tx-PD0
#define DEBUG_UART1_Remap2    3  //Tx-PD6
#define DEBUG_UART1_Remap3    4  //Tx-PC0

#ifdef NVIC_PriorityGroup_0
	#define select_NVIC_PriorityGroup	NVIC_PriorityGroup_0
#else
	#define select_NVIC_PriorityGroup	NVIC_PriorityGroup_1
#endif


/* MCU Selection */
#if (SelectMCU  == CH32V003F) // TSSOP20
	
	#define _port_triac	GPIOC
	#define _pin_triac	GPIO_Pin_1

	#define _port_zc			GPIOC
	#define _pin_zc				GPIO_Pin_1
	#define	_line_zc			EXTI_Line1
	#define _exti_config_zc		GPIO_PortSourceGPIOC, GPIO_PinSource1

	#define _port_enc	GPIOC
	#define _pin_enc	GPIO_Pin_5

    #define _port_pwm	GPIOC
	#define _pin_pwm	GPIO_Pin_4

    #ifndef DEBUG
    #define DEBUG   DEBUG_UART1_NoRemap
    #endif
	
#elif (SelectMCU  == CH32V003J) // SO8
	#define _port_triac	GPIOA
	#define _pin_triac	GPIO_Pin_2

	#define _port_zc			GPIOC
	#define _pin_zc				GPIO_Pin_1
	#define	_line_zc			EXTI_Line1
	#define _exti_config_zc		GPIO_PortSourceGPIOC, GPIO_PinSource1

	#define _port_enc	GPIOC
	#define _pin_enc	GPIO_Pin_2

    #define _port_pwm	GPIOC
	#define _pin_pwm	GPIO_Pin_4

    #ifndef DEBUG
    #define DEBUG   DEBUG_UART1_Remap2
    #endif

#endif

#define selectRCC(port)	((port==GPIOA)?RCC_APB2Periph_GPIOA:\
						((port==GPIOC)?RCC_APB2Periph_GPIOC:\
						((port==GPIOD)?RCC_APB2Periph_GPIOD:(-1))))

#define isPwm()	(GPIO_ReadInputDataBit(_port_pwm ,_pin_pwm ))
#define isEnc()	(GPIO_ReadInputDataBit(_port_enc ,_pin_enc ))
#define isZc()	(GPIO_ReadInputDataBit(_port_zc , _pin_zc ))
#define Triac(b) ( GPIO_WriteBit(_port_triac, _pin_triac, b))



/*********************************************************************
 * @fn      Init_CPU
 *
 * @brief   Initializes CPU general. NVIC and Core Clock Init
 *
 * @param   
 *
 * @return  none
 */
extern void Init_CPU(void);
/*********************************************************************
 * @fn      Init_TriacOut
 *
 * @brief   Initializes GPIO Output. Use for Triac Gate drive
 *
 * @param   
 *
 * @return  none
 */
extern void Init_TriacOut(void);

/*********************************************************************
 * @fn      Input_Capture_Init
 *
 * @brief   Initializes TIM1 input capture. Use for pwm input
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          cb - interrupt callback function.
 *
 * @return  none
 */
extern void Input_Capture_Init(u16 arr, u16 psc, void (*cb)(uint16_t ris, uint16_t fal  ));

/*********************************************************************
 * @fn      Input_Capture_Init2
 *
 * @brief   Initializes TIM1 input capture. Use for measure periode time
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          cb - interrupt callback function.
 *
 * @return  none
 */
extern void Input_Capture_Init2(u16 arr, u16 psc, void (*cb)(uint16_t ris, uint16_t fal ));

/*********************************************************************
 * @fn      EXTI_app_Init
 *
 * @brief   Initializes GPIO Input and Interrupt. Use for Zerocross detect
 *
 * @param   Interrupt call back function
 *
 * @return  none
 */
extern void EXTI_app_Init( void* cb);

/*********************************************************************
 * @fn   	ADC_HW_Init
 * @brief   Initializes ADC and ADC Interrupt. Use for Some analog values measure
 *
 * @param   cb_rdy - Interrupt call back function , all adc input measured
 *			_chs - Channels list pointer
 *			_nbrs - Channels list length
 *			_rslts - Result values write list pointer
 *
 * @return  none
 */
extern void ADC_HW_Init(void (*cb_rdy)(void ), uint8_t *_chs, uint8_t _nbrs, long *_rslts);

/*********************************************************************
 * @fn   	Get_ADC_Val
 * @brief   Standalone convert one shot from one channel.
 *
 * @param   _chs - Channel number
 *
 * @return  measured relevant channel analog value
 */
extern unsigned short Get_ADC_Val(unsigned char ch);

#endif /* USER_INIT_PCUT_WCH_H_ */
