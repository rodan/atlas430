
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
#include "qa.h"
#include "timer_a0.h"
#include "uart3.h"

void main_init(void)
{
    // port init
    // debug signals
    P1DIR |= BIT0 + BIT3 + BIT4;

#ifdef USE_XT1
    PJSEL0 |= BIT4 | BIT5;
    CS_setExternalClockSource(32768, 0);
#endif

    // Set DCO Frequency to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    // Set DCO Frequency to 1MHz
    //CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_0);

    // configure MCLK, SMCLK to be sourced by DCOCLK
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

#ifdef USE_XT1
    CS_turnOnLFXT(CS_LFXT_DRIVE_3);
#endif
}

static void uart0_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart0_set_eol();
}

static void uart3_rx_irq(uint32_t msg)
{
    parse_sensor_output();
    uart3_set_eol();
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
    timer_a0_init();
    uart0_port_init();
    uart0_init();
    uart3_port_init();
    uart3_init();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);
    eh_register(&uart3_rx_irq, SYS_MSG_UART3_RX);

    display_menu();

    sig1_on;
    sig2_on;
    sig3_on;

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        //led_switch;
        check_events();
    }
}
