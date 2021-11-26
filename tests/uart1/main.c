
/*
    program that tests the functionality of the EUSCI A0 UART 

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"
#include "sig.h"

static void uart1_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart1_set_eol();
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart1_get_event() == UART1_EV_RX) {
        msg |= SYS_MSG_UART1_RX;
        uart1_rst_event();
    }

    eh_exec(msg);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
#ifdef USE_SIG
    sig0_on;
#endif
    clock_init();
    uart1_pin_init();
    uart1_init();

#ifdef UART1_RX_USES_RINGBUF
    uart1_set_rx_irq_handler(uart1_rx_ringbuf_handler);
#else
    uart1_set_rx_irq_handler(uart1_rx_simple_handler);
#endif

#ifdef USE_SIG
    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
#endif

    eh_init();
    eh_register(&uart1_rx_irq, SYS_MSG_UART1_RX);
    _BIS_SR(GIE);

    display_version();

    while (1) {
#ifdef USE_SIG
        sig4_off;
#endif
        // sleep
        _BIS_SR(LPM0_bits + GIE);
#ifdef USE_SIG
        sig4_on;
#endif
        __no_operation();
        check_events();
        check_events();
        check_events();
    }
}
