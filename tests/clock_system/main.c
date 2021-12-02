
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
    PMM_unlockLPM5();
#endif
    sig0_on;

    clock_pin_init();
    clock_init();

    // output SMCLK on a pin
#if defined (__CC430F5137__)
    P3DIR |= BIT7;
    P3SEL |= BIT7;
#elif defined (__MSP430FR5994__) || ((__MSP430FR5969__))
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;
#elif defined (__MSP430FR2355__)
    P3DIR |= BIT4;
    P3SEL0 |= BIT4;
    P3SEL1 &= ~BIT4;
#elif defined (__MSP430FR2433__) || (__MSP430FR2476__)
    P1DIR |= BIT7;
    P1SEL0 &= ~BIT7;
    P1SEL1 |= BIT7;
#elif defined (__MSP430FR4133__)
    P8DIR |= BIT0;
    P8SEL0 |= BIT0;
#elif defined (__MSP430FR6989__)
    // on the EXP430FR6989 devboard P10.0 is ties to an LCD segment, so don't output SMCLK to it
    //P10DIR |= BIT0;
    //P10SEL0 |= BIT0;
    //P10SEL1 |= BIT0;
#elif defined (__MSP430F5510__) || (__MSP430F5529__)
    P2DIR |= BIT2;
    P2SEL |= BIT2;
#endif

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;

    while (1) {
        __no_operation();
        __delay_cycles(SMCLK_FREQ / 2);
        sig0_switch;
    }
}
