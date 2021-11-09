
#include <msp430.h>
#include "proj.h"
#include "driverlib.h"
#include "glue.h"

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;

    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
    sig0_on;

    clock_port_init();
    clock_init();

    #if defined (__MSP430FR5994__)
    // output SMCLK on P3.4
    P3OUT &= ~BIT4;
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;
    PM5CTL0 &= ~LOCKLPM5;
    #endif

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;

    while (1) {
        // sleep
        //_BIS_SR(LPM3_bits + GIE);
        __no_operation();
        __delay_cycles(8000000);
        sig1_switch;
    }
}

