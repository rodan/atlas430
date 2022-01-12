
/*
    program that replicates an IR signal received via a Vishay TSOP IR receiver

    tweak the uart baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"
#include "timer_a0.h"
#include "ir_acquire.h"
#include "sig.h"

uart_descriptor bc; // backchannel uart interface

static void uart_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart_set_eol(&bc);
}

static void acquisition_end(uint32_t msg)
{
    qa_acquisition_end();
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart_get_event(&bc) == UART_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart_rst_event(&bc);
    }

    // timer
    if (timer_a0_get_event()) {
        msg |= timer_a0_get_event();
        timer_a0_rst_event();
    }

    eh_exec(msg);
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
#ifdef USE_SIG
    sig0_on;
#endif

    clock_pin_init();
    clock_init();

    bc.baseAddress = EUSCI_A0_BASE;
    bc.baudrate = BAUDRATE_57600;
    uart_pin_init(&bc);
    uart_init(&bc);
#if defined UART_RX_USES_RINGBUF
    uart_set_rx_irq_handler(&bc, uart_rx_ringbuf_handler);
#else
    uart_set_rx_irq_handler(&bc, uart_rx_simple_handler);
#endif

    timer_a0_init();

#ifdef USE_SIG
    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
#endif

    eh_init();
    eh_register(&uart_rx_irq, SYS_MSG_UART0_RX);
    eh_register(&acquisition_end, SYS_MSG_TIMER0_CRR2);
    _BIS_SR(GIE);

    display_version();

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


