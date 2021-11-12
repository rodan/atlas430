/*
  HAL setup that currently covers the gpio initialization
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430
*/

#include <stdint.h>
#include <msp430.h>
#include "pmm.h"
#include "msp430_hal.h"

void msp430_hal_init(const uint8_t gpio_flags)
{
    uint8_t dir = 0; // by default the pin is floating
    uint8_t out = 0; // default logic level is low if pin is output

    if (gpio_flags & HAL_GPIO_DIR_OUTPUT) {
        dir = 0xff;
    }

    if (gpio_flags & HAL_GPIO_OUT_HIGH) {
        out = 0xff;
    }

    // Turn everything off, device drivers enable what is needed.
    // All configured for digital I/O
#ifdef __MSP430_HAS_P1SEL1__
    P1SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P2SEL1__
    P2SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P3SEL1__
    P3SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P4SEL1__
    P4SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P5SEL1__
    P5SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P6SEL1__
    P6SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P7SEL1__
    P7SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P8SEL1__
    P8SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P9SEL1__
    P9SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P10SEL1__
    P10SEL1 = 0;
#endif
#ifdef __MSP430_HAS_P11SEL1__
    P11SEL1 = 0;
#endif

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

    PMM_unlockLPM5();
}
