
/*
    program that tests the functionality of the EUSCI A0 UART 

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "config.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"

void main_init(void)
{
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
}

static void uart0_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart0_set_eol();
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart0_get_event() == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_rst_event();
    }

    eh_exec(msg);
}

int main(void)
{
    //char buf[CONV_BASE_2_BUF_SZ];

    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();
    sig0_on;

    clock_port_init();
    clock_init();

    uart0_port_init();
    uart0_init();

#ifdef UART0_RX_USES_RINGBUF
    uart0_set_rx_irq_handler(uart0_rx_ringbuf_handler);
#else
    uart0_set_rx_irq_handler(uart0_rx_simple_handler);
#endif

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;

    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);

    display_menu();

    while (1) {
        // sleep
#ifdef LED_SYSTEM_STATES
        sig4_off;
#endif
        _BIS_SR(LPM3_bits + GIE);
#ifdef LED_SYSTEM_STATES
        sig4_on;
#endif
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
    }
}
