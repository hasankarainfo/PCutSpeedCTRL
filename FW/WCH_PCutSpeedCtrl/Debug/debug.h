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
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <ch32v00x.h>
#include <stdio.h>
#include <stdbool.h>
#include "diff_int.h"
#include "hybos_threads.h"
#include "init_wch.h"



/* allocate buffer space for print_debug */
#define print_blength  128

extern long sys_tick_us;
extern long sys_tick_ms;

extern int debug_printf( const char *__restrict, ...);

/*********************************************************************
 * @fn      Delay_Init
 *
 * @brief   Initializes Delay Funcation.
 *
 * @return  none
 */
void Delay_Init(void);
/*********************************************************************
 * @fn      Delay_Us
 *
 * @brief   Microsecond Delay Time.
 *
 * @param   n - Microsecond number.
 *
 * @return  None
 */
void Delay_Us(uint32_t n);
void Delay_Wait_Us();
diff_max_t Delay_Peek_Us(void);
/*********************************************************************
 * @fn      Delay_Set_Us
 *
 * @brief   Micro second timer set
 *
* @param   n - Offset relative to current time.
 *
 * @return  none
 */
void Delay_Set_Us(uint32_t n);
/*********************************************************************
 * @fn      Delay_Ms
 *
 * @brief   Millisecond Delay Time.
 *
 * @param   n - Millisecond number.
 *
 * @return  None
 */
void Delay_Ms(uint32_t n);
void Delay_Wait_Ms();
diff_max_t Delay_Peek_Ms(void);
/*********************************************************************
 * @fn      Delay_Set_Ms
 *
 * @brief   Mili second timer set
 *
* @param   n - Offset relative to current time.
 *
 * @return  none
 */
void Delay_Set_Ms(uint32_t n);
/*********************************************************************
 * @fn      USART_Printf_Init
 *
 * @brief   Initializes the USARTx peripheral.
 *
 * @param   hyb_parent - Parent task pointer.
 *          baudrate - USART communication baud rate.
 *
 * @return  None
 */
hyb_ts* USART_Printf_Init(hyb_ts *hyb_parent, uint32_t baudrate);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_H */
