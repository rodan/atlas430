
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

void main_init(void)
{
    msp430_hal_init();
}

static void uart2_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart2_set_eol();
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart2_get_event() == UART2_EV_RX) {
        msg |= SYS_MSG_UART2_RX;
        uart2_rst_event();
    }

    eh_exec(msg);
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();
    P1DIR |= BIT0;
    sig0_on;

    clock_port_init();
    clock_init();

#if defined (__MSP430FR5994__)
    // output SMCLK on P3.4
    //P3OUT &= ~BIT4;
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;
#elif defined (__MSP430F5438__)
    // output SMCLK on P1.6
    P1DIR |= BIT6;
    P1SEL |= BIT6;
#elif defined (__MSP430F5510__) || defined (__MSP430F5529__)
    // output SMCLK on P2.2
    P2DIR |= BIT2;
    P2SEL |= BIT2;
#endif

    timer_a0_init();

    uart2_port_init();
    uart2_init();

#ifdef UART2_RX_USES_RINGBUF
    uart2_set_rx_irq_handler(uart2_rx_ringbuf_handler);
#else
    uart2_set_rx_irq_handler(uart2_rx_simple_handler);
#endif

//#if (defined(USE_XT2) && defined(SMCLK_FREQ_16M)) || defined(UART2_TX_USES_IRQ)
    // an external high frequency crystal can't be woken up quickly enough
    // from LPM, so make sure that SMCLK never powers down

    // also the uart tx irq ain't working without this for some reason
    timer_a0_init();
//#endif

#if defined (__MSP430FR5994__)
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
#endif

    sig0_on;
    sig1_off;
    sig2_off;
    sig3_off;
#ifdef LED_SYSTEM_STATES
    sig4_on;
#else
    sig4_off;
#endif

    eh_init();
    eh_register(&uart2_rx_irq, SYS_MSG_UART2_RX);

//#define TEST_UART2_TX_STR
//#define TEST_UART2_PRINT
//#define TEST_ITOA
//#define TEST_SNPRINTF
//#define TEST_UTOH
//#define TEST_UTOB

#ifdef TEST_UART2_TX_STR
    uart2_tx_str("h1llo world\r\n", 13);
    uart2_tx_str("he2lo world\r\n", 13);
    uart2_tx_str("hel3o world\r\n", 13);
    uart2_tx_str("hell4 world\r\n", 13);
    uart2_tx_str("hello5world\r\n", 13);
    uart2_tx_str("hello 6orld\r\n", 13);
    uart2_tx_str("hello w7rld\r\n", 13);
    uart2_tx_str("hello wo8ld\r\n", 13);
    uart2_tx_str("hello wor9d\r\n", 13);
    uart2_tx_str("hello worl0\r\n", 13);
#endif

#ifdef TEST_UART2_PRINT
    uart2_print("h1llo world\r\n");
    uart2_print("he2lo world\r\n");
    uart2_print("hel3o world\r\n");
    uart2_print("hell4 world\r\n");
    uart2_print("hello5world\r\n");
    uart2_print("hello 6orld\r\n");
    uart2_print("hello w7rld\r\n");
    uart2_print("hello wo8ld\r\n");
    uart2_print("hello wor9d\r\n");
    uart2_print("hello worl0\r\n");
#endif

#ifdef TEST_ITOA
    uart2_print(_itoa(buf, 0));
    uart2_print("\r\n");
    uart2_print(_itoa(buf, 65535));
    uart2_print("\r\n");
    uart2_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 11));
    uart2_print("\r\n");
    uart2_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 12));
    uart2_print("\r\n");
    uart2_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 13));
    uart2_print("\r\n");
    uart2_print(_utoa(buf, 4294967295));
    uart2_print("\r\n");
    uart2_print(_itoa(buf, -2147483647));
    uart2_print("\r\n");
    uart2_print(_itoa(buf, -65535));
    uart2_print("\r\n");
    uart2_print(_itoa(buf, -1));
    uart2_print("\r\n");
#endif

#ifdef TEST_SNPRINTF
    snprintf(buf, 12, "%d", 0);
    uart2_print(buf);
    snprintf(buf, 12, "%ld", 65535);
    uart2_print(buf);
    snprintf(buf, 12, "%lld", 4294967295);
    uart2_print(buf);
    snprintf(buf, 12, "%ld", -65535);
    uart2_print(buf);
    snprintf(buf, 12, "%d", -1);
    uart2_print(buf);
#endif

#ifdef TEST_UTOH
    uart2_print(_utoh(&buf[0], 0));
    uart2_print("\r\n");
    uart2_print(_utoh(&buf[0], 0x00000fef));
    uart2_print("\r\n");
    uart2_print(_utoh(&buf[0], 0xffffffff));
    uart2_print("\r\n");
    uart2_print(_utoh(&buf[0], 0x0110cafe));
    uart2_print("\r\n");
    uart2_print(_utoh(&buf[0], 0xa1b2c3e4));
    uart2_print("\r\n");
#endif

#ifdef TEST_UTOB
    uart2_print(_utob(&buf[0], 0));
    uart2_print("\r\n");
    uart2_print(_utob(&buf[0], 0xffff));
    uart2_print("\r\n");
    uart2_print(_utob(&buf[0], 0xe));
    uart2_print("\r\n");
    uart2_print(_utob(&buf[0], 0x010a));
    uart2_print("\r\n");
    uart2_print(_utob(&buf[0], 0xefef));
    uart2_print("\r\n");
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
