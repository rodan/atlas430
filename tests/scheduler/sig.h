#ifndef __SIG_H__
#define __SIG_H__

#include <msp430.h>

#if defined (__CC430F5137__) || defined (__MSP430FR5994__) || defined (__MSP430FR6989__) || defined (__MSP430FR4133__) || defined (__MSP430FR2355__) || defined (__MSP430FR2433__) || defined (__MSP430FR2476__)

#define sig0_on              P1OUT |= BIT0
#define sig0_off             P1OUT &= ~BIT0
#define sig0_switch          P1OUT ^= BIT0

#else
    #error "define a sig0_switch for your particular board"
#endif

#endif
