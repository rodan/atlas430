/*
  HAL setup that currently covers the gpio initialization
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430
*/

#include <stdint.h>
#include <msp430.h>
#include "msp430_hal.h"

void msp430_hal_init(const uint8_t gpio_flags)
{
    uint8_t dir = 0; // by default the pin is floating
    uint8_t out = 0; // default logic level is low if pin is output

    if (gpio_flags & HAL_GPIO_DIR_OUTPUT) {
        dir = 1;
    }

    if (gpio_flags & HAL_GPIO_OUT_HIGH) {
        out = 1;
    }

    // Turn everything off, device drivers enable what is needed.
    // All configured for digital I/O
#ifdef __MSP430_HAS_P1SEL__
    P1SEL = 0;
#endif
#ifdef __MSP430_HAS_P2SEL__
    P2SEL = 0;
#endif
#ifdef __MSP430_HAS_P3SEL__
    P3SEL = 0;
#endif
#ifdef __MSP430_HAS_P4SEL__
    P4SEL = 0;
#endif
#ifdef __MSP430_HAS_P5SEL__
    P5SEL = 0;
#endif
#ifdef __MSP430_HAS_P6SEL__
    P6SEL = 0;
#endif
#ifdef __MSP430_HAS_P7SEL__
    P7SEL = 0;
#endif
#ifdef __MSP430_HAS_P8SEL__
    P8SEL = 0;
#endif
#ifdef __MSP430_HAS_P9SEL__
    P9SEL = 0;
#endif
#ifdef __MSP430_HAS_P10SEL__
    P10SEL = 0;
#endif
#ifdef __MSP430_HAS_P11SEL__
    P11SEL = 0;
#endif

    // make all gpios inputs
#ifdef __MSP430_HAS_PORT1_R__
    P1DIR = dir;
    P1OUT = out;
#endif
#ifdef __MSP430_HAS_PORT2_R__
    P2DIR = dir;
    P2OUT = out;
#endif
#ifdef __MSP430_HAS_PORT3_R__
    P3DIR = dir;
    P3OUT = out;
#endif
#ifdef __MSP430_HAS_PORT4_R__
    P4DIR = dir;
    P4OUT = out;
#endif
#ifdef __MSP430_HAS_PORT5_R__
    P5DIR = dir;
    P5OUT = out;
#endif
#ifdef __MSP430_HAS_PORT6_R__
    P6DIR = dir;
    P6OUT = out;
#endif
#ifdef __MSP430_HAS_PORT7_R__
    P7DIR = dir;
    P7OUT = out;
#endif
#ifdef __MSP430_HAS_PORT8_R__
    P8DIR = dir;
    P8OUT = out;
#endif
#ifdef __MSP430_HAS_PORT9_R__
    P9DIR = dir;
    P9OUT = out;
#endif
#ifdef __MSP430_HAS_PORT10_R__
    P10DIR = dir;
    P10OUT = out;
#endif
#ifdef __MSP430_HAS_PORT11_R__
    P11DIR = dir;
    P11OUT = out;
#endif
}
