#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>

#if defined (__MSP430F5438__)
#define sig0_on              P10OUT |= BIT6
#define sig0_off             P10OUT &= ~BIT6
#define sig0_switch          P10OUT ^= BIT6

#define sig1_on              P10OUT |= BIT7
#define sig1_off             P10OUT &= ~BIT7
#define sig1_switch          P10OUT ^= BIT7

#else
#define sig0_on              P1OUT |= BIT0
#define sig0_off             P1OUT &= ~BIT0
#define sig0_switch          P1OUT ^= BIT0

#define sig1_on              P1OUT |= BIT1
#define sig1_off             P1OUT &= ~BIT1
#define sig1_switch          P1OUT ^= BIT1
#endif

#define sig2_on              P1OUT |= BIT3
#define sig2_off             P1OUT &= ~BIT3
#define sig2_switch          P1OUT ^= BIT3

#define sig3_on              P1OUT |= BIT4
#define sig3_off             P1OUT &= ~BIT4
#define sig3_switch          P1OUT ^= BIT4

#define sig4_on              P1OUT |= BIT5
#define sig4_off             P1OUT &= ~BIT5
#define sig4_switch          P1OUT ^= BIT5

#endif
