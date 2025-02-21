#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>

#define true                1
#define false               0

/*!
	\brief List of possible message types for the event handler.
	\sa eh_register()
*/
#define           SYS_MSG_NULL  0
#define    SYS_MSG_UART_BCL_RX  0x1   // UART received something

#define        FM24_SLAVE_ADDR  0x50
#define     TCA6408_SLAVE_ADDR  0x21

// HSCMRNN030PA2A3 chip
#define         HSC_SLAVE_ADDR  0x28

#endif
