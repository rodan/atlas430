
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
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
    sig0_on;

    clock_init();

    uart1_pin_init();
    uart1_init();

#ifdef UART1_RX_USES_RINGBUF
    uart1_set_rx_irq_handler(uart1_rx_ringbuf_handler);
#else
    uart1_set_rx_irq_handler(uart1_rx_simple_handler);
#endif

#if (defined(USE_XT2) && defined(SMCLK_FREQ_16M)) || defined(UART1_TX_USES_IRQ)
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
    eh_register(&uart1_rx_irq, SYS_MSG_UART1_RX);

//#define TEST_UART1_TX_STR
//#define TEST_UART1_PRINT
//#define TEST_ITOA
//#define TEST_SNPRINTF
//#define TEST_UTOH
//#define TEST_UTOB

#ifdef TEST_UART1_TX_STR
    uart1_tx_str("h1llo world\r\n", 13);
    uart1_tx_str("he2lo world\r\n", 13);
    uart1_tx_str("hel3o world\r\n", 13);
    uart1_tx_str("hell4 world\r\n", 13);
    uart1_tx_str("hello5world\r\n", 13);
    uart1_tx_str("hello 6orld\r\n", 13);
    uart1_tx_str("hello w7rld\r\n", 13);
    uart1_tx_str("hello wo8ld\r\n", 13);
    uart1_tx_str("hello wor9d\r\n", 13);
    uart1_tx_str("hello worl0\r\n", 13);
#endif

#ifdef TEST_UART1_PRINT
    uart1_print("h1llo world\r\n");
    uart1_print("he2lo world\r\n");
    uart1_print("hel3o world\r\n");
    uart1_print("hell4 world\r\n");
    uart1_print("hello5world\r\n");
    uart1_print("hello 6orld\r\n");
    uart1_print("hello w7rld\r\n");
    uart1_print("hello wo8ld\r\n");
    uart1_print("hello wor9d\r\n");
    uart1_print("hello worl0\r\n");
#endif

#ifdef TEST_ITOA
    uart1_print(_itoa(buf, 0));
    uart1_print("\r\n");
    uart1_print(_itoa(buf, 65535));
    uart1_print("\r\n");
    uart1_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 11));
    uart1_print("\r\n");
    uart1_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 12));
    uart1_print("\r\n");
    uart1_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 13));
    uart1_print("\r\n");
    uart1_print(_utoa(buf, 4294967295));
    uart1_print("\r\n");
    uart1_print(_itoa(buf, -2147483647));
    uart1_print("\r\n");
    uart1_print(_itoa(buf, -65535));
    uart1_print("\r\n");
    uart1_print(_itoa(buf, -1));
    uart1_print("\r\n");
#endif

#ifdef TEST_SNPRINTF
    snprintf(buf, 12, "%d", 0);
    uart1_print(buf);
    snprintf(buf, 12, "%ld", 65535);
    uart1_print(buf);
    snprintf(buf, 12, "%lld", 4294967295);
    uart1_print(buf);
    snprintf(buf, 12, "%ld", -65535);
    uart1_print(buf);
    snprintf(buf, 12, "%d", -1);
    uart1_print(buf);
#endif

#ifdef TEST_UTOH
    uart1_print(_utoh(&buf[0], 0));
    uart1_print("\r\n");
    uart1_print(_utoh(&buf[0], 0x00000fef));
    uart1_print("\r\n");
    uart1_print(_utoh(&buf[0], 0xffffffff));
    uart1_print("\r\n");
    uart1_print(_utoh(&buf[0], 0x0110cafe));
    uart1_print("\r\n");
    uart1_print(_utoh(&buf[0], 0xa1b2c3e4));
    uart1_print("\r\n");
#endif

#ifdef TEST_UTOB
    uart1_print(_utob(&buf[0], 0));
    uart1_print("\r\n");
    uart1_print(_utob(&buf[0], 0xffff));
    uart1_print("\r\n");
    uart1_print(_utob(&buf[0], 0xe));
    uart1_print("\r\n");
    uart1_print(_utob(&buf[0], 0x010a));
    uart1_print("\r\n");
    uart1_print(_utob(&buf[0], 0xefef));
    uart1_print("\r\n");
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
