
#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "atlas430.h"
#include "exf.h"
#include "sig.h"

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
    sig0_on;

    clock_pin_init();
    clock_init();
    sig0_off;

    sig1_on;
    exf();
    sig1_off;

    while (1) {
        _BIS_SR(LPM3_bits + GIE);
    }
}
