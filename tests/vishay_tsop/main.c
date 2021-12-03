
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

static void uart0_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart0_set_eol();
}

static void acquisition_end(uint32_t msg)
{
    qa_acquisition_end();
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart0_get_event() == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_rst_event();
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

    uart0_pin_init();
    uart0_init();

#ifdef UART0_RX_USES_RINGBUF
    uart0_set_rx_irq_handler(uart0_rx_ringbuf_handler);
#else
    uart0_set_rx_irq_handler(uart0_rx_simple_handler);
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
    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);
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


