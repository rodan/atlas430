/*
  pin setup for uart UCA1 subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on Sun Jan  9 06:16:19 UTC 2022
*/

#include <msp430.h>

void uart_uca1_pin_init(void)
{

#ifdef UART_USES_UCA1

#if defined (__MSP430FR2433__) || defined (__MSP430FR2475__) \
 || defined (__MSP430FR2476__) || defined (__MSP430FR2532__) \
 || defined (__MSP430FR2533__) || defined (__MSP430FR2632__) \
 || defined (__MSP430FR2633__) || defined (__MSP430FR2672__) \
 || defined (__MSP430FR2673__) || defined (__MSP430FR2675__) \
 || defined (__MSP430FR2676__)

    P2SEL0 |= BIT5 | BIT6;
    P2SEL1 &= ~(BIT5 | BIT6);

#elif defined (__MSP430FR2153__) || defined (__MSP430FR2155__) \
 || defined (__MSP430FR2353__) || defined (__MSP430FR2355__)

    P4SEL0 |= BIT2 | BIT3;
    P4SEL1 &= ~(BIT2 | BIT3);

#elif defined (__MSP430FR2000__) || defined (__MSP430FR2032__) \
 || defined (__MSP430FR2033__) || defined (__MSP430FR2100__) \
 || defined (__MSP430FR2110__) || defined (__MSP430FR2111__) \
 || defined (__MSP430FR2310__) || defined (__MSP430FR2311__) \
 || defined (__MSP430FR2422__) || defined (__MSP430FR2512__) \
 || defined (__MSP430FR2522__) || defined (__MSP430FR4131__) \
 || defined (__MSP430FR4132__) || defined (__MSP430FR4133__)

#warning function UCA1RXD not found for this uC
#warning function UCA1TXD not found for this uC

#else
    #warning "UART_USES_UCA1 was defined but pins not known in 'glue/MSP430FR2xx_4xx/uart_uca1_pin.c'"
#endif
#endif
}
