/*
  pin setup for UCA3 subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430

  generated on Mon Nov 22 16:34:12 UTC 2021
*/

#include <msp430.h>

void uart3_pin_init(void)
{

#ifdef USE_UART3

#if defined (__MSP430FR5962__) || defined (__MSP430FR5964__) \
 || defined (__MSP430FR5992__) || defined (__MSP430FR59941__) \
 || defined (__MSP430FR5994__)

    P6SEL0 |= BIT0 | BIT1;
    P6SEL1 &= ~(BIT0 | BIT1);

#elif defined (__MSP430FR5041__) || defined (__MSP430FR50431__) \
 || defined (__MSP430FR5043__) || defined (__MSP430FR6041__) \
 || defined (__MSP430FR60431__) || defined (__MSP430FR6043__)

    P2SEL0 &= ~(BIT0 | BIT1);
    P2SEL1 |= BIT0 | BIT1;

#elif defined (__MSP430FR6005__) || defined (__MSP430FR6007__) \
 || defined (__MSP430FR6035__) || defined (__MSP430FR60371__) \
 || defined (__MSP430FR6037__) || defined (__MSP430FR6045__) \
 || defined (__MSP430FR60471__) || defined (__MSP430FR6047__)

    P8SEL0 |= BIT2 | BIT3;
    P8SEL1 &= ~(BIT2 | BIT3);

#else
    #error "USE_UART3 was defined but pins not known in 'glue/MSP430FR5xx_6xx/uart3_pin.c'"
#endif
#endif
}
