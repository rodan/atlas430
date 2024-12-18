/*
  chip select pin setup for the spi UCB0 slave subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on Fri Mar 11 19:04:33 UTC 2022
*/

#include <msp430.h>

void spi_cs_ucb0_pin_init(void)
{

#ifdef SPI_USES_UCB0

#if defined (__MSP430FR2032__) || defined (__MSP430FR2033__) \
 || defined (__MSP430FR4131__) || defined (__MSP430FR4132__) \
 || defined (__MSP430FR4133__)

    P5DIR &= ~BIT0;
    P5SEL0 |= BIT0;

#elif defined (__MSP430FR2310__) || defined (__MSP430FR2311__)

    P1SEL0 |= BIT0;
    P1SEL1 &= ~BIT0;
    P2SEL0 |= BIT2;
    P2SEL1 &= ~BIT2;
#warning multiple pins found for the UCB0STE function, you must initialize them manually

#elif defined (__MSP430FR2475__) || defined (__MSP430FR2476__) \
 || defined (__MSP430FR2672__) || defined (__MSP430FR2673__) \
 || defined (__MSP430FR2675__) || defined (__MSP430FR2676__)

    P1SEL0 |= BIT0;
    P1SEL1 &= ~BIT0;
    P5SEL0 |= BIT6;
    P5SEL1 &= ~BIT6;
#warning multiple pins found for the UCB0STE function, you must initialize them manually

#elif defined (__MSP430FR2153__) || defined (__MSP430FR2155__) \
 || defined (__MSP430FR2353__) || defined (__MSP430FR2355__) \
 || defined (__MSP430FR2433__) || defined (__MSP430FR2532__) \
 || defined (__MSP430FR2533__) || defined (__MSP430FR2632__) \
 || defined (__MSP430FR2633__)

    P1SEL0 |= BIT0;
    P1SEL1 &= ~BIT0;

#elif defined (__MSP430FR2422__) || defined (__MSP430FR2512__) \
 || defined (__MSP430FR2522__)

    P1SEL0 |= BIT0;
    P1SEL1 &= ~BIT0;
    P2SEL0 &= ~BIT3;
    P2SEL1 |= BIT3;
#warning multiple pins found for the UCB0STE function, you must initialize them manually

#elif defined (__MSP430FR2000__) || defined (__MSP430FR2100__) \
 || defined (__MSP430FR2110__) || defined (__MSP430FR2111__)

#warning function UCB0STE not found for this uC

#else
    #warning "SPI_USES_UCB0 was defined but CS pins not known in 'glue/MSP430FR2xx_4xx/spi_cs_ucb0_pin.c'"
#endif

#endif
}
