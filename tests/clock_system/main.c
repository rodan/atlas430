
#include <msp430.h>
#include "proj.h"
#include "driverlib.h"
#include "glue.h"

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;

    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
    #if defined (__MSP430_HAS_FRAM__)
    hhhh
    PMM_unlockLPM5();
    #endif
    sig0_on;

    clock_port_init();
    clock_init();

    // output SMCLK on a pin
    #if defined (__MSP430FR5994__)
    P3OUT &= ~BIT4;
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;
    #elif defined (__MSP430FR2355__)
    P3DIR |= BIT4;
    P3SEL0 |= BIT4;
    P3SEL1 &= ~BIT4;
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

