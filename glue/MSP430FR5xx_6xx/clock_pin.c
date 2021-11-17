
#include <msp430.h>
#include "clock_pin.h"

// this file is generated automatically by a datasheet parser

// USE_XT1 has to be defined if the LFXTCLK (32768 Hz) crystal is present
// USE_XT2 has to be defined if the HFXTCLK crystal is present

void clock_pin_init(void)
{
#ifdef USE_XT1
    #if defined (__MSP430FR5994__) || (__MSP430FR5969__) || (__MSP430FR6989__)
    PJSEL0 |= BIT4 | BIT5;
    #else
    #error "USE_XT1 was defined but pins not known in 'glue/MSP430FR5xx_6xx/clock.c'"
    #endif
#endif

#ifdef USE_XT2
    #if defined (__MSP430FR5994__) || (__MSP430FR5969__) || (__MSP430FR6989__)
    PJSEL0 |= BIT6 | BIT7;
    #else
    #error "USE_XT2 was defined but pins not known in 'glue/MSP430FR5xx_6xx/clock.c'"
    #endif
#endif
}

