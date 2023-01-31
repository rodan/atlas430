#ifndef __SIG_H__
#define __SIG_H__

#include <msp430.h>

#if defined (__CC430F5137__) || defined (__MSP430FR5994__) || defined (__MSP430FR6989__) || defined (__MSP430FR2355__) || defined (__MSP430FR2433__) || defined (__MSP430FR2476__) || defined (__MSP430F5529__) || defined (__MSP430FR5969__)

#define sig0_on              P1OUT |= BIT0
#define sig0_off             P1OUT &= ~BIT0
#define sig0_switch          P1OUT ^= BIT0

#elif defined (__MSP430F5510__)

#define sig0_on              P4OUT |= BIT7
#define sig0_off             P4OUT &= ~BIT7
#define sig0_switch          P4OUT ^= BIT7

#elif defined (__MSP430FR4133__)

#define sig0_on              P4OUT |= BIT0
#define sig0_off             P4OUT &= ~BIT0
#define sig0_switch          P4OUT ^= BIT0

#elif defined (__MSP430F5438__)

#define sig0_on              P10OUT |= BIT7
#define sig0_off             P10OUT &= ~BIT7
#define sig0_switch          P10OUT ^= BIT7


#else
    #error "define a sig0_switch for your particular board"
#endif

#endif
