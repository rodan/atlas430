/*
  pin setup for crystal-based clocks. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430

  generated on Tue Nov 23 07:55:30 UTC 2021
*/

#include <msp430.h>

void clock_pin_init(void)
{
#ifdef USE_XT1

#if defined (__MSP430FR2433__) || defined (__MSP430FR2475__) \
 || defined (__MSP430FR2476__) || defined (__MSP430FR2532__) \
 || defined (__MSP430FR2533__) || defined (__MSP430FR2632__) \
 || defined (__MSP430FR2633__) || defined (__MSP430FR2672__) \
 || defined (__MSP430FR2673__) || defined (__MSP430FR2675__) \
 || defined (__MSP430FR2676__)

    P2SEL0 |= BIT0 | BIT1;
    P2SEL1 &= ~(BIT0 | BIT1);

#elif defined (__MSP430FR2000__) || defined (__MSP430FR2100__) \
 || defined (__MSP430FR2110__) || defined (__MSP430FR2111__) \
 || defined (__MSP430FR2153__) || defined (__MSP430FR2155__) \
 || defined (__MSP430FR2310__) || defined (__MSP430FR2311__) \
 || defined (__MSP430FR2353__) || defined (__MSP430FR2355__)

    P2SEL0 &= ~(BIT6 | BIT7);
    P2SEL1 |= BIT6 | BIT7;

#elif defined (__MSP430FR2032__) || defined (__MSP430FR2033__) \
 || defined (__MSP430FR4131__) || defined (__MSP430FR4132__) \
 || defined (__MSP430FR4133__)

    P4SEL0 |= BIT1 | BIT2;

#elif defined (__MSP430FR2422__) || defined (__MSP430FR2512__) \
 || defined (__MSP430FR2522__)

    P2SEL0 &= ~(BIT0 | BIT1);
    P2SEL1 |= BIT0 | BIT1;

#else
    #error "USE_XT1 was defined but pins not known in 'glue/MSP430FR2xx_4xx/clock_pin.c'"
#endif
#endif
}
