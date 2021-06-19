
/*
    program that tests the functionality of the EUSCI A0 UART 

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"

void main_init(void)
{
    // set all ports as output, low level

    P1OUT = 0;
    P1DIR = 0xFF;

    P2OUT = 0;
    P2DIR = 0xFF;

    P3OUT = 0;
    P3DIR = 0xFF;

    P4OUT = 0;
    P4DIR = 0xFF;

    P5OUT = 0;
    P5DIR = 0xFF;

    P6OUT = 0;
    P6DIR = 0xFF;

    P7OUT = 0;
    P7DIR = 0xFF;

    P8DIR = 0xFF;
    P8OUT = 0;

    PJOUT = 0;
    PJDIR = 0xFFFF;
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

    AD7789_init(EUSCI_SPI_BASE_ADDR);

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;

    eh_init();
    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);
    display_menu();

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
    }
}
