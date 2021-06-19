#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"

#define sig0_on              P1OUT |= BIT0
#define sig0_off             P1OUT &= ~BIT0
#define sig0_switch          P1OUT ^= BIT0

#define sig1_on              P1OUT |= BIT1
#define sig1_off             P1OUT &= ~BIT1
#define sig1_switch          P1OUT ^= BIT1

#define sig2_on              P1OUT |= BIT3
#define sig2_off             P1OUT &= ~BIT3
#define sig2_switch          P1OUT ^= BIT3

#define sig3_on              P1OUT |= BIT4
#define sig3_off             P1OUT &= ~BIT4
#define sig3_switch          P1OUT ^= BIT4

#define sig4_on              P1OUT |= BIT5
#define sig4_off             P1OUT &= ~BIT5
#define sig4_switch          P1OUT ^= BIT5

#define DS3234_CS_HIGH      P5OUT |= BIT3
#define DS3234_CS_LOW       P5OUT &= ~BIT3

#define true                1
#define false               0

/*!
	\brief List of possible message types for the message bus.
	\sa sys_messagebus_register()
*/

#define           SYS_MSG_NULL 0
#define    SYS_MSG_TIMER0_CRR1 0x1   // timer_a0_delay_noblk_ccr1
#define    SYS_MSG_TIMER0_CRR2 0x2   // timer_a0_delay_noblk_ccr2
#define     SYS_MSG_TIMER0_IFG 0x4   // timer0 overflow
#define       SYS_MSG_UART0_RX 0x8   // UART received something
#define        SYS_MSG_P53_INT 0x10  // ds3234 interrupt

#endif
