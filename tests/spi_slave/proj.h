#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>

#define CS_PIN      BIT3

#define led_on              P1OUT |= BIT0
#define led_off             P1OUT &= ~BIT0
#define led_switch          P1OUT ^= BIT0

#define true                1
#define false               0

/*!
	\brief List of possible message types for the event handler.
	\sa eh_register()
*/

#define           SYS_MSG_NULL 0
#define    SYS_MSG_TIMER0_CRR1 0x1   // timer_a0_delay_noblk_ccr1
#define    SYS_MSG_TIMER0_CRR2 0x2   // timer_a0_delay_noblk_ccr2
#define     SYS_MSG_TIMER0_IFG 0x4   // timer0 overflow
#define       SYS_MSG_UART0_RX 0x8   // UART received something

#endif
