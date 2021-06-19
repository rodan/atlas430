
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

#include "timer_a0.h"

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
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();
    sig0_on;

    clock_port_init();
    clock_init();

    // output SMCLK on P3.4
    P3OUT &= ~BIT4;
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;

    uart0_port_init();
    uart0_init();

#ifdef UART0_RX_USES_RINGBUF
    uart0_set_rx_irq_handler(uart0_rx_ringbuf_handler);
#else
    uart0_set_rx_irq_handler(uart0_rx_simple_handler);
#endif

#if (defined(USE_XT2) && defined(SMCLK_FREQ_16M)) || defined(UART0_TX_USES_IRQ)
    // an external high frequency crystal can't be woken up quickly enough
    // from LPM, so make sure that SMCLK never powers down

    // also the uart tx irq ain't working without this for some reason
    timer_a0_init();
#endif

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
#ifdef LED_SYSTEM_STATES
    sig4_on;
#else
    sig4_off;
#endif

    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);

//#define TEST_UART0_TX_STR
//#define TEST_UART0_PRINT
//#define TEST_ITOA
//#define TEST_SNPRINTF
//#define TEST_UTOH
//#define TEST_UTOB

#ifdef TEST_UART0_TX_STR
    uart0_tx_str("h1llo world\r\n", 13);
    uart0_tx_str("he2lo world\r\n", 13);
    uart0_tx_str("hel3o world\r\n", 13);
    uart0_tx_str("hell4 world\r\n", 13);
    uart0_tx_str("hello5world\r\n", 13);
    uart0_tx_str("hello 6orld\r\n", 13);
    uart0_tx_str("hello w7rld\r\n", 13);
    uart0_tx_str("hello wo8ld\r\n", 13);
    uart0_tx_str("hello wor9d\r\n", 13);
    uart0_tx_str("hello worl0\r\n", 13);
#endif

#ifdef TEST_UART0_PRINT
    uart0_print("h1llo world\r\n");
    uart0_print("he2lo world\r\n");
    uart0_print("hel3o world\r\n");
    uart0_print("hell4 world\r\n");
    uart0_print("hello5world\r\n");
    uart0_print("hello 6orld\r\n");
    uart0_print("hello w7rld\r\n");
    uart0_print("hello wo8ld\r\n");
    uart0_print("hello wor9d\r\n");
    uart0_print("hello worl0\r\n");
#endif

#ifdef TEST_ITOA
    uart0_print(_itoa(buf, 0));
    uart0_print("\r\n");
    uart0_print(_itoa(buf, 65535));
    uart0_print("\r\n");
    uart0_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 11));
    uart0_print("\r\n");
    uart0_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 12));
    uart0_print("\r\n");
    uart0_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 13));
    uart0_print("\r\n");
    uart0_print(_utoa(buf, 4294967295));
    uart0_print("\r\n");
    uart0_print(_itoa(buf, -2147483647));
    uart0_print("\r\n");
    uart0_print(_itoa(buf, -65535));
    uart0_print("\r\n");
    uart0_print(_itoa(buf, -1));
    uart0_print("\r\n");
//>> Building main.elf as target RELEASE
//   text    data     bss     dec     hex filename
//   2627     212     106    2945     b81 main.elf
#endif

#ifdef TEST_SNPRINTF
    snprintf(buf, 12, "%d", 0);
    uart0_print(buf);
    snprintf(buf, 12, "%ld", 65535);
    uart0_print(buf);
    snprintf(buf, 12, "%lld", 4294967295);
    uart0_print(buf);
    snprintf(buf, 12, "%ld", -65535);
    uart0_print(buf);
    snprintf(buf, 12, "%d", -1);
    uart0_print(buf);
//>> Building main.elf as target RELEASE
//   text    data     bss     dec     hex filename
//   5151     212     106    5469    155d main.elf
#endif

#ifdef TEST_UTOH
    uart0_print(_utoh(&buf[0], 0));
    uart0_print("\r\n");
    uart0_print(_utoh(&buf[0], 0x00000fef));
    uart0_print("\r\n");
    uart0_print(_utoh(&buf[0], 0xffffffff));
    uart0_print("\r\n");
    uart0_print(_utoh(&buf[0], 0x0110cafe));
    uart0_print("\r\n");
    uart0_print(_utoh(&buf[0], 0xa1b2c3e4));
    uart0_print("\r\n");
#endif

#ifdef TEST_UTOB
    uart0_print(_utob(&buf[0], 0));
    uart0_print("\r\n");
    uart0_print(_utob(&buf[0], 0xffff));
    uart0_print("\r\n");
    uart0_print(_utob(&buf[0], 0xe));
    uart0_print("\r\n");
    uart0_print(_utob(&buf[0], 0x010a));
    uart0_print("\r\n");
    uart0_print(_utob(&buf[0], 0xefef));
    uart0_print("\r\n");
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
