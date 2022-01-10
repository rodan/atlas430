#ifndef __SIG_H__
#define __SIG_H__

#include <msp430.h>

#define sig3_on              P4OUT |= BIT0
#define sig3_off             P4OUT &= ~BIT0
#define sig3_switch          P4OUT ^= BIT0

#define sig0_on              P4OUT |= BIT1
#define sig0_off             P4OUT &= ~BIT1
#define sig0_switch          P4OUT ^= BIT1

#define sig1_on              P4OUT |= BIT2
#define sig1_off             P4OUT &= ~BIT2
#define sig1_switch          P4OUT ^= BIT2

#define sig2_on              P4OUT |= BIT3
#define sig2_off             P4OUT &= ~BIT3
#define sig2_switch          P4OUT ^= BIT3

#define sig4_on              P2OUT |= BIT0
#define sig4_off             P2OUT &= ~BIT0
#define sig4_switch          P2OUT ^= BIT0

#endif
