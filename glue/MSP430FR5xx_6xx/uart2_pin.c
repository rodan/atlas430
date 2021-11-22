/*
  pin setup for UCA2 subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430

  generated on Mon Nov 22 16:33:34 UTC 2021
*/

#include <msp430.h>

void uart2_pin_init(void)
{

#ifdef USE_UART2

#if defined (__MSP430FR5962__) || defined (__MSP430FR5964__) \
 || defined (__MSP430FR5992__) || defined (__MSP430FR59941__) \
 || defined (__MSP430FR5994__)

    P5SEL0 |= BIT4 | BIT5;
    P5SEL1 &= ~(BIT4 | BIT5);

#elif defined (__MSP430FR5041__) || defined (__MSP430FR50431__) \
 || defined (__MSP430FR5043__) || defined (__MSP430FR6005__) \
 || defined (__MSP430FR6007__) || defined (__MSP430FR6035__) \
 || defined (__MSP430FR60371__) || defined (__MSP430FR6037__) \
 || defined (__MSP430FR6041__) || defined (__MSP430FR60431__) \
 || defined (__MSP430FR6043__) || defined (__MSP430FR6045__) \
 || defined (__MSP430FR60471__) || defined (__MSP430FR6047__)

    P5SEL0 &= ~(BIT0 | BIT1);
    P5SEL1 |= BIT0 | BIT1;

#else
    #error "USE_UART2 was defined but pins not known in 'glue/MSP430FR5xx_6xx/uart2_pin.c'"
#endif
#endif
}
