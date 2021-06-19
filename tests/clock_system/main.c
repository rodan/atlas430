
#include <msp430.h>
#include "proj.h"
#include "config.h"
#include "driverlib.h"
#include "glue.h"
#include "timer_a0.h"

void main_init(void);

int main(void)
{
    main_init();

    while (1) {
        // sleep
        //_BIS_SR(LPM3_bits + GIE);
        __no_operation();
        __delay_cycles(8000000);
        sig1_switch;
    }
}

void main_init(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;

    // port init
    P1OUT = 0;
    P1DIR = 0xff;

    P2OUT = 0;
    P2DIR = 0xff;

    P3OUT = 0;
    P3DIR = 0xff;

    P4OUT = 0;
    P4DIR = 0xff;

    // P55 and P56 are buttons
    P5OUT = 0;
    P5DIR = 0x9f;
    // activate pullup
    P5OUT = 0x60;
    P5REN = 0x60;
    // IRQ triggers on the falling edge
    P5IES = 0x60;

    P6OUT = 0;
    P6DIR = 0xff;

    P7OUT = 0;
    P7DIR = 0xff;

    P8OUT = 0;
    P8DIR = 0xff;

    PJOUT = 0;
    PJDIR = 0xffff;
    sig0_on;

    clock_port_init();
    clock_init();

    // output SMCLK on P3.4
    P3OUT &= ~BIT4;
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
}


