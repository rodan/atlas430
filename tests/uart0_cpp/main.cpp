
#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"

void main_init(void)
{
    // port init
    P1DIR = BIT0;
}

static void uart0_rx_irq(uint32_t msg)
{
    //parse_user_input();
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
    char buf[64];

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

//#define TEST_UART0_TX_STR
//#define TEST_UART0_PRINT
//#define TEST_ITOA
//#define TEST_SNPRINTF
#define TEST_UTOH
//#define TEST_UTOB

#ifdef TEST_UART0_TX_STR
    uart0_tx_str("h1llo world\r\n",13);
    uart0_tx_str("he2lo world\r\n",13);
    uart0_tx_str("hel3o world\r\n",13);
    uart0_tx_str("hell4 world\r\n",13);
    uart0_tx_str("hello5world\r\n",13);
    uart0_tx_str("hello 6orld\r\n",13);
    uart0_tx_str("hello w7rld\r\n",13);
    uart0_tx_str("hello wo8ld\r\n",13);
    uart0_tx_str("hello wor9d\r\n",13);
    uart0_tx_str("hello worl0\r\n",13);
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
    uart0_print(_itoa(&buf[0], 0));
    uart0_print("\r\n");
    uart0_print(_itoa(&buf[0], 65535));
    uart0_print("\r\n");
    uart0_print(_utoa(&buf[0], 4294967295));
    uart0_print("\r\n");
    uart0_print(_itoa(&buf[0], -65535));
    uart0_print("\r\n");
    uart0_print(_itoa(&buf[0], -1));
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

    while (1) {
        // sleep
        sig2_off;
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        sig2_on;
        check_events();
    }
}
