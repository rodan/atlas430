#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>
#include <stdlib.h>
#include <inttypes.h>

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
#define       SYS_MSG_UART1_RX 0x8   // UART received something

#endif
