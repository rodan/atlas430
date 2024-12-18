/*
  pin setup for the spi UCB1 subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on Tue Jan  4 14:23:24 UTC 2022
*/

#include <msp430.h>

void spi_ucb1_pin_init(void)
{

#ifdef SPI_USES_UCB1

#if defined (__MSP430FR2000__) || defined (__MSP430FR2032__) \
 || defined (__MSP430FR2033__) || defined (__MSP430FR2100__) \
 || defined (__MSP430FR2110__) || defined (__MSP430FR2111__) \
 || defined (__MSP430FR2310__) || defined (__MSP430FR2311__) \
 || defined (__MSP430FR2422__) || defined (__MSP430FR2433__) \
 || defined (__MSP430FR2512__) || defined (__MSP430FR2522__) \
 || defined (__MSP430FR2532__) || defined (__MSP430FR2533__) \
 || defined (__MSP430FR2632__) || defined (__MSP430FR2633__) \
 || defined (__MSP430FR4131__) || defined (__MSP430FR4132__) \
 || defined (__MSP430FR4133__)

#warning function UCB1CLK not found for this uC
#warning function UCB1SIMO not found for this uC
#warning function UCB1SOMI not found for this uC

#elif defined (__MSP430FR2153__) || defined (__MSP430FR2155__) \
 || defined (__MSP430FR2353__) || defined (__MSP430FR2355__)

    P4SEL0 |= BIT5 | BIT6 | BIT7;
    P4SEL1 &= ~(BIT5 | BIT6 | BIT7);

#elif defined (__MSP430FR2475__) || defined (__MSP430FR2476__) \
 || defined (__MSP430FR2672__) || defined (__MSP430FR2673__) \
 || defined (__MSP430FR2675__) || defined (__MSP430FR2676__)

    P3SEL0 |= BIT2 | BIT5 | BIT6;
    P3SEL1 &= ~(BIT2 | BIT5 | BIT6);
    P4SEL0 |= BIT3 | BIT4;
    P4SEL1 &= ~(BIT3 | BIT4);
    P5SEL0 |= BIT3;
    P5SEL1 &= ~BIT3;
#warning multiple pins found for the UCB1CLK function, you must initialize them manually
#warning multiple pins found for the UCB1SIMO function, you must initialize them manually
#warning multiple pins found for the UCB1SOMI function, you must initialize them manually

#else
    #warning "SPI_USES_UCB1 was defined but pins not known in 'glue/MSP430FR2xx_4xx/spi_ucb1_pin.c'"
#endif

#endif
}
