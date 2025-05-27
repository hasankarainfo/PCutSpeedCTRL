/********************************** (C) COPYRIGHT  *******************************
 * File Name          : debug.c
 * Author             : hasankarainfo
 * Version            : V_01b
 * Date               : 2025/04/15
 * Description        : UART Init, Uart circular buffer puts task create, 
                        Delay, systick, debug_printf functions.
 *********************************************************************************
* Copyright (c) hasankarainfo
 *******************************************************************************/
#include <debug.h>

#include <ctype.h>

#include "stdio.h"
#include <stdlib.h>
#include "stdarg.h"
///\\\-\---\----\------\-------\--------\----------\-----------\-------------

int print_fcnt=0;
int print_rcnt=0;
char print_buf[print_blength];

uint8_t  p_us = 0;
uint16_t p_ms = 0;
long sys_tick_us = 0;
long sys_tick_ms = 0;
void (*sys_tick_cb)(void) = 0;

hyb_ts	*hyb_printf;


long df=0 ;
static diff_ts dt_ms, dt_us;

#define us_precision	20 /*is to lower finest precision*/
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Delay_Init(void)
{
	diff_init_def(&dt_ms, &sys_tick_ms);
	diff_init_def(&dt_us, &sys_tick_us);

//    p_us = SystemCoreClock / 8000000;
	p_us = SystemCoreClock / 1000000;
    p_ms = (uint16_t)p_us * 1000;

    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->SR &= ~1;
    SysTick->CMP = p_us*us_precision;
    SysTick->CNT = 0;
    SysTick->CTLR = 0xF;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
extern void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick_Handler(void)
{
	NVIC_ClearPendingIRQ(SysTicK_IRQn);

    SysTick->CMP = p_us * us_precision;
//    SysTick->CNT = 0;
    SysTick->SR &= ~1;
    SysTick->CTLR = 0xF;
    sys_tick_us += us_precision;
    if(sys_tick_us % 1000 == 0)sys_tick_ms++;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Delay_Set_Ms(uint32_t n){
	diff_set(&dt_ms, n);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
diff_max_t Delay_Peek_Ms(void){
	return (diff_peek(&dt_ms) );
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Delay_Wait_Ms(){
	while(Delay_Peek_Ms()< 0  );
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Delay_Set_Us(uint32_t n){
	diff_set(&dt_us, n);
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
diff_max_t Delay_Peek_Us(void){
	return (diff_peek(&dt_us) );
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Delay_Wait_Us(){
	while(Delay_Peek_Us()< 0  );
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Delay_Us(uint32_t n)
{
	Delay_Set_Us(n);
	Delay_Wait_Us();
}

///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void Delay_Ms(uint32_t n)
{
	Delay_Set_Ms(n);
	Delay_Wait_Ms();
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
int debug_printf( const char *fmt, ...)
{
    char _buf[print_blength]; 
    int _size;
    int _cnt = 0;
    static int capacity ;
    /* calculate buffer capacity*/
    capacity = (print_rcnt + print_blength-1 - print_fcnt)%(print_blength);
    if(capacity==0)return 0;

    va_list ap;
    va_start(ap, fmt);
    _size = vsprintf(_buf, fmt, ap );
    va_end(ap);

    if(capacity<_size) _size = capacity;

    while(_cnt<_size  ){
        print_buf[print_fcnt++]= _buf[_cnt++];
        if(print_fcnt >= print_blength )print_fcnt = 0;
    }
    hyb_notify(hyb_printf,0);
    
    return _cnt;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
void task_printf(hyb_ts *hyb){
	hyb_begin;
	 while (1){
        
        while(print_fcnt != print_rcnt){
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE ))hyb_wait(200); /* unit micro second*/
            USART_SendData(USART1, print_buf[print_rcnt++]);
            if(print_rcnt >= print_blength )print_rcnt = 0;
    	}
        hyb_suspend();
	 }
	hyb_end;
}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------


hyb_ts* USART_Printf_Init( hyb_ts *hyb_parent, uint32_t baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
 
#if (DEBUG == DEBUG_UART1_NoRemap)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
     
  
#elif (DEBUG == DEBUG_UART1_Remap1)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap2)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap2_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap3)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

#endif

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    hyb_printf = hyb_thread_create(hyb_parent, &task_printf,0,0);
    return hyb_printf;

}
///\\\-\---\----\------\-------\--------\----------\-----------\-------------
