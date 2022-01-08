#ifndef __SIG_H__
#define __SIG_H__

#include <msp430.h>

#define sig0_on              P2OUT |= BIT1
#define sig0_off             P2OUT &= ~BIT1
#define sig0_switch          P2OUT ^= BIT1

#define sig1_on              P2OUT |= BIT0
#define sig1_off             P2OUT &= ~BIT0
#define sig1_switch          P2OUT ^= BIT0

#define sig2_on              P1OUT |= BIT5
#define sig2_off             P1OUT &= ~BIT5
#define sig2_switch          P1OUT ^= BIT5

#define sig3_on              P1OUT |= BIT6
#define sig3_off             P1OUT &= ~BIT6
#define sig3_switch          P1OUT ^= BIT6

#define sig4_on              P1OUT |= BIT7
#define sig4_off             P1OUT &= ~BIT7
#define sig4_switch          P1OUT ^= BIT7

#define sig5_on              P3OUT |= BIT2
#define sig5_off             P3OUT &= ~BIT2
#define sig5_switch          P3OUT ^= BIT2

#endif
