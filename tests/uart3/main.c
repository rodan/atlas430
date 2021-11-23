
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

#include "timer_a0.h"

static void uart3_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart3_set_eol();
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart3_get_event() == UART3_EV_RX) {
        msg |= SYS_MSG_UART3_RX;
        uart3_rst_event();
    }

    eh_exec(msg);
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
    sig0_on;

    clock_init();

    uart3_pin_init();
    uart3_init();

#ifdef UART3_RX_USES_RINGBUF
    uart3_set_rx_irq_handler(uart3_rx_ringbuf_handler);
#else
    uart3_set_rx_irq_handler(uart3_rx_simple_handler);
#endif

#if (defined(USE_XT2) && defined(SMCLK_FREQ_16M)) || defined(UART3_TX_USES_IRQ)
    // an external high frequency crystal can't be woken up quickly enough
    // from LPM, so make sure that SMCLK never powers down

    // also the uart tx irq ain't working without this for some reason
    timer_a0_init();
#endif

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
#ifdef LED_SYSTEM_STATES
    sig4_on;
#else
    sig4_off;
#endif

    eh_init();
    eh_register(&uart3_rx_irq, SYS_MSG_UART3_RX);

//#define TEST_UART3_TX_STR
//#define TEST_UART3_PRINT
//#define TEST_ITOA
//#define TEST_SNPRINTF
//#define TEST_UTOH
//#define TEST_UTOB

#ifdef TEST_UART3_TX_STR
    uart3_tx_str("h1llo world\r\n", 13);
    uart3_tx_str("he2lo world\r\n", 13);
    uart3_tx_str("hel3o world\r\n", 13);
    uart3_tx_str("hell4 world\r\n", 13);
    uart3_tx_str("hello5world\r\n", 13);
    uart3_tx_str("hello 6orld\r\n", 13);
    uart3_tx_str("hello w7rld\r\n", 13);
    uart3_tx_str("hello wo8ld\r\n", 13);
    uart3_tx_str("hello wor9d\r\n", 13);
    uart3_tx_str("hello worl0\r\n", 13);
#endif

#ifdef TEST_UART3_PRINT
    uart3_print("h1llo world\r\n");
    uart3_print("he2lo world\r\n");
    uart3_print("hel3o world\r\n");
    uart3_print("hell4 world\r\n");
    uart3_print("hello5world\r\n");
    uart3_print("hello 6orld\r\n");
    uart3_print("hello w7rld\r\n");
    uart3_print("hello wo8ld\r\n");
    uart3_print("hello wor9d\r\n");
    uart3_print("hello worl0\r\n");
#endif

#ifdef TEST_ITOA
    uart3_print(_itoa(buf, 0));
    uart3_print("\r\n");
    uart3_print(_itoa(buf, 65535));
    uart3_print("\r\n");
    uart3_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 11));
    uart3_print("\r\n");
    uart3_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 12));
    uart3_print("\r\n");
    uart3_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 13));
    uart3_print("\r\n");
    uart3_print(_utoa(buf, 4294967295));
    uart3_print("\r\n");
    uart3_print(_itoa(buf, -2147483647));
    uart3_print("\r\n");
    uart3_print(_itoa(buf, -65535));
    uart3_print("\r\n");
    uart3_print(_itoa(buf, -1));
    uart3_print("\r\n");
#endif

#ifdef TEST_SNPRINTF
    snprintf(buf, 12, "%d", 0);
    uart3_print(buf);
    snprintf(buf, 12, "%ld", 65535);
    uart3_print(buf);
    snprintf(buf, 12, "%lld", 4294967295);
    uart3_print(buf);
    snprintf(buf, 12, "%ld", -65535);
    uart3_print(buf);
    snprintf(buf, 12, "%d", -1);
    uart3_print(buf);
#endif

#ifdef TEST_UTOH
    uart3_print(_utoh(&buf[0], 0));
    uart3_print("\r\n");
    uart3_print(_utoh(&buf[0], 0x00000fef));
    uart3_print("\r\n");
    uart3_print(_utoh(&buf[0], 0xffffffff));
    uart3_print("\r\n");
    uart3_print(_utoh(&buf[0], 0x0110cafe));
    uart3_print("\r\n");
    uart3_print(_utoh(&buf[0], 0xa1b2c3e4));
    uart3_print("\r\n");
#endif

#ifdef TEST_UTOB
    uart3_print(_utob(&buf[0], 0));
    uart3_print("\r\n");
    uart3_print(_utob(&buf[0], 0xffff));
    uart3_print("\r\n");
    uart3_print(_utob(&buf[0], 0xe));
    uart3_print("\r\n");
    uart3_print(_utob(&buf[0], 0x010a));
    uart3_print("\r\n");
    uart3_print(_utob(&buf[0], 0xefef));
    uart3_print("\r\n");
#endif

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
        check_events();
        check_events();
    }
}
