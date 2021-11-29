/*
  pin setup for UCA3 subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on Wed Nov 24 06:56:11 UTC 2021
*/

#include <msp430.h>

void uart3_pin_init(void)
{

#ifdef USE_UART3

#if defined (__MSP430BT5190__) || defined (__MSP430F5418__) \
 || defined (__MSP430F5418A__) || defined (__MSP430F5419__) \
 || defined (__MSP430F5419A__) || defined (__MSP430F5435__) \
 || defined (__MSP430F5435A__) || defined (__MSP430F5436__) \
 || defined (__MSP430F5436A__) || defined (__MSP430F5437__) \
 || defined (__MSP430F5437A__) || defined (__MSP430F5438__) \
 || defined (__MSP430F5438A__)

    P10SEL |= BIT4 | BIT5;

#elif defined (__MSP430F67451__) || defined (__MSP430F67451A__) \
 || defined (__MSP430F6745__) || defined (__MSP430F6745A__) \
 || defined (__MSP430F67461__) || defined (__MSP430F67461A__) \
 || defined (__MSP430F6746__) || defined (__MSP430F6746A__) \
 || defined (__MSP430F67471__) || defined (__MSP430F67471A__) \
 || defined (__MSP430F6747__) || defined (__MSP430F6747A__) \
 || defined (__MSP430F67481__) || defined (__MSP430F67481A__) \
 || defined (__MSP430F6748__) || defined (__MSP430F6748A__) \
 || defined (__MSP430F67491__) || defined (__MSP430F67491A__) \
 || defined (__MSP430F6749__) || defined (__MSP430F6749A__) \
 || defined (__MSP430F67651__) || defined (__MSP430F67651A__) \
 || defined (__MSP430F6765__) || defined (__MSP430F6765A__) \
 || defined (__MSP430F67661__) || defined (__MSP430F67661A__) \
 || defined (__MSP430F6766__) || defined (__MSP430F6766A__) \
 || defined (__MSP430F67671__) || defined (__MSP430F67671A__) \
 || defined (__MSP430F6767__) || defined (__MSP430F6767A__) \
 || defined (__MSP430F67681__) || defined (__MSP430F67681A__) \
 || defined (__MSP430F6768__) || defined (__MSP430F6768A__) \
 || defined (__MSP430F67691__) || defined (__MSP430F67691A__) \
 || defined (__MSP430F6769__) || defined (__MSP430F6769A__) \
 || defined (__MSP430F67751__) || defined (__MSP430F67751A__) \
 || defined (__MSP430F6775__) || defined (__MSP430F6775A__) \
 || defined (__MSP430F67761__) || defined (__MSP430F67761A__) \
 || defined (__MSP430F6776__) || defined (__MSP430F6776A__) \
 || defined (__MSP430F67771__) || defined (__MSP430F67771A__) \
 || defined (__MSP430F6777__) || defined (__MSP430F6777A__) \
 || defined (__MSP430F67781__) || defined (__MSP430F67781A__) \
 || defined (__MSP430F6778__) || defined (__MSP430F6778A__) \
 || defined (__MSP430F67791__) || defined (__MSP430F67791A__) \
 || defined (__MSP430F6779__) || defined (__MSP430F6779A__)

    P4SEL0 |= BIT1 | BIT2;

#elif defined (__MSP430F5252__) || defined (__MSP430F5253__) \
 || defined (__MSP430F5254__) || defined (__MSP430F5255__) \
 || defined (__MSP430F5256__) || defined (__MSP430F5257__) \
 || defined (__MSP430F5258__) || defined (__MSP430F5259__)

    P4SEL |= BIT6 | BIT7;

#else
    #error "USE_UART3 was defined but pins not known in 'glue/MSP430F5xx_6xx/uart3_pin.c'"
#endif
#endif
}
