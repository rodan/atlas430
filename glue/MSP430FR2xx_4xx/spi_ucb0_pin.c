/*
  pin setup for the spi UCB0 subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on Tue Jan  4 14:23:19 UTC 2022
*/

#include <msp430.h>

void spi_ucb0_pin_init(void)
{

#ifdef SPI_USES_UCB0

#if defined (__MSP430FR2000__) || defined (__MSP430FR2100__) \
 || defined (__MSP430FR2110__) || defined (__MSP430FR2111__)

#warning function UCB0CLK not found for this uC
#warning function UCB0SIMO not found for this uC
#warning function UCB0SOMI not found for this uC

#elif defined (__MSP430FR2422__) || defined (__MSP430FR2512__) \
 || defined (__MSP430FR2522__)

    P1SEL0 |= BIT1 | BIT2 | BIT3;
    P1SEL1 &= ~(BIT1 | BIT2 | BIT3);
    P2SEL0 &= ~(BIT4 | BIT5 | BIT6);
    P2SEL1 |= BIT4 | BIT5 | BIT6;
#warning multiple pins found for the UCB0CLK function, you must initialize them manually
#warning multiple pins found for the UCB0SIMO function, you must initialize them manually
#warning multiple pins found for the UCB0SOMI function, you must initialize them manually

#elif defined (__MSP430FR2475__) || defined (__MSP430FR2476__) \
 || defined (__MSP430FR2672__) || defined (__MSP430FR2673__) \
 || defined (__MSP430FR2675__) || defined (__MSP430FR2676__)

    P1SEL0 |= BIT1 | BIT2 | BIT3;
    P1SEL1 &= ~(BIT1 | BIT2 | BIT3);
    P4SEL0 |= BIT5 | BIT6;
    P4SEL1 &= ~(BIT5 | BIT6);
    P5SEL0 |= BIT5;
    P5SEL1 &= ~BIT5;
#warning multiple pins found for the UCB0CLK function, you must initialize them manually
#warning multiple pins found for the UCB0SIMO function, you must initialize them manually
#warning multiple pins found for the UCB0SOMI function, you must initialize them manually

#elif defined (__MSP430FR2433__) || defined (__MSP430FR2532__) \
 || defined (__MSP430FR2533__) || defined (__MSP430FR2632__) \
 || defined (__MSP430FR2633__)

    P1SEL0 |= BIT1 | BIT2 | BIT3;
    P1SEL1 &= ~(BIT1 | BIT2 | BIT3);

#elif defined (__MSP430FR2310__) || defined (__MSP430FR2311__)

    P1SEL0 |= BIT1 | BIT2 | BIT3;
    P1SEL1 &= ~(BIT1 | BIT2 | BIT3);
    P2SEL0 |= BIT3 | BIT4 | BIT5;
    P2SEL1 &= ~(BIT3 | BIT4 | BIT5);
#warning multiple pins found for the UCB0CLK function, you must initialize them manually
#warning multiple pins found for the UCB0SIMO function, you must initialize them manually
#warning multiple pins found for the UCB0SOMI function, you must initialize them manually

#elif defined (__MSP430FR2032__) || defined (__MSP430FR2033__) \
 || defined (__MSP430FR4131__) || defined (__MSP430FR4132__) \
 || defined (__MSP430FR4133__)

    P5DIR &= ~(BIT1 | BIT2 | BIT3);
    P5SEL0 |= BIT1 | BIT2 | BIT3;

#elif defined (__MSP430FR2153__) || defined (__MSP430FR2155__) \
 || defined (__MSP430FR2353__) || defined (__MSP430FR2355__)

    P1SEL0 |= BIT1 | BIT2 | BIT3;
    P1SEL1 &= ~(BIT1 | BIT2 | BIT3);
#warning multiple pins found for the UCB0CLK function, you must initialize them manually

#else
    #warning "SPI_USES_UCB0 was defined but pins not known in 'glue/MSP430FR2xx_4xx/spi_ucb0_pin.c'"
#endif

#endif
}
