#ifndef __EXF_H__
#define __EXF_H__

#include <msp430.h>

#define clk_high             P3OUT |= BIT0
#define clk_low              P3OUT &= ~BIT0

#define mosi_high            P3OUT |= BIT1
#define mosi_low             P3OUT &= ~BIT1

#define cs_high              P3OUT |= BIT2
#define cs_low               P3OUT &= ~BIT2

void exf_init(void);
void exf(void);

#endif
