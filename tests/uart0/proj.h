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
#define       SYS_MSG_UART0_RX 0x1   // UART received something

#endif
