
#include <stdio.h>
#include <string.h>
#include "glue.h"
#include "version.h"
#include "uart_mapping.h"
#include "ui.h"

//#define TEST_TX_STR
//#define TEST_PRINT
//#define TEST_ITOA
//#define TEST_SNPRINTF
//#define TEST_UTOH
//#define TEST_UTOB

static const char menu_str[]="\
 available commands:\r\n\r\n\
\e[33;1m?\e[0m  - show menu\r\n\
\e[33;1mt\e[0m  - run selected test\r\n\
\e[33;1ma\e[0m  - run 'lotsa text' test\r\n\
\e[33;1md\e[0m  - convert input into decimal\r\n\
\e[33;1mh\e[0m  - convert input into hex\r\n";
static const char err_conv_str[]="error during str_to_int32()\r\n";
static const char received_str[]="received ";

void display_menu(void)
{
    display_version();
    uart_bcl_print(menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_bcl_print("uartx b");
    uart_bcl_print(_utoa(sconv, BUILD));
    uart_bcl_print("\r\n");
}

void display_test(void)
{
#if defined TEST_ITOA || defined TEST_SNPRINTF || defined TEST_UTOH || defined TEST_UTOB
    char buf[CONV_BASE_10_BUF_SZ];
#endif

#ifdef TEST_TX_STR
    uart_bcl_tx_str("h1llo world\r\n", 13);
    uart_bcl_tx_str("he2lo world\r\n", 13);
    uart_bcl_tx_str("hel3o world\r\n", 13);
    uart_bcl_tx_str("hell4 world\r\n", 13);
    uart_bcl_tx_str("hello5world\r\n", 13);
    uart_bcl_tx_str("hello 6orld\r\n", 13);
    uart_bcl_tx_str("hello w7rld\r\n", 13);
    uart_bcl_tx_str("hello wo8ld\r\n", 13);
    uart_bcl_tx_str("hello wor9d\r\n", 13);
    uart_bcl_tx_str("hello worl0\r\n", 13);
#endif

#ifdef TEST_PRINT
    uart_bcl_print("h1llo world\r\n"); // 13
    uart_bcl_print("he2lo world\r\n"); // 26
    uart_bcl_print("hel3o world\r\n"); // 40
    uart_bcl_print("hell4 world\r\n"); // 56
    uart_bcl_print("hello5world\r\n");
    uart_bcl_print("hello 6orld\r\n");
    uart_bcl_print("hello w7rld\r\n");
    uart_bcl_print("hello wo8ld\r\n");
    uart_bcl_print("hello wor9d\r\n");
    uart_bcl_print("hello worl0\r\n");
#endif

#ifdef TEST_ITOA
    uart_bcl_print(_itoa(buf, 0));
    uart_bcl_print("\r\n");
    uart_bcl_print(_itoa(buf, 65535));
    uart_bcl_print("\r\n");
    uart_bcl_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 11));
    uart_bcl_print("\r\n");
    uart_bcl_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 12));
    uart_bcl_print("\r\n");
    uart_bcl_print(prepend_padding(buf, _itoa(buf, 1234), PAD_ZEROES, 13));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utoa(buf, 4294967295));
    uart_bcl_print("\r\n");
    uart_bcl_print(_itoa(buf, -2147483647));
    uart_bcl_print("\r\n");
    uart_bcl_print(_itoa(buf, -65535));
    uart_bcl_print("\r\n");
    uart_bcl_print(_itoa(buf, -1));
    uart_bcl_print("\r\n");
#endif

#ifdef TEST_SNPRINTF
    snprintf(buf, 12, "%d", 0);
    uart_bcl_print(buf);
    snprintf(buf, 12, "%ld", 65535);
    uart_bcl_print(buf);
    snprintf(buf, 12, "%lld", 4294967295);
    uart_bcl_print(buf);
    snprintf(buf, 12, "%ld", -65535);
    uart_bcl_print(buf);
    snprintf(buf, 12, "%d", -1);
    uart_bcl_print(buf);
#endif

#ifdef TEST_UTOH
    uart_bcl_print(_utoh(&buf[0], 0));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utoh(&buf[0], 0x00000fef));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utoh(&buf[0], 0xffffffff));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utoh(&buf[0], 0x0110cafe));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utoh(&buf[0], 0xa1b2c3e4));
    uart_bcl_print("\r\n");
#endif

#ifdef TEST_UTOB
    uart_bcl_print(_utob(&buf[0], 0));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utob(&buf[0], 0xffff));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utob(&buf[0], 0xe));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utob(&buf[0], 0x010a));
    uart_bcl_print("\r\n");
    uart_bcl_print(_utob(&buf[0], 0xefef));
    uart_bcl_print("\r\n");
#endif
}

#define PARSER_CNT 16

void parse_user_input(void)
{
#if defined UART0_RX_USES_RINGBUF || defined UART1_RX_USES_RINGBUF || \
    defined UART2_RX_USES_RINGBUF || defined UART3_RX_USES_RINGBUF
    struct ringbuf *rbr = uart_bcl_get_rx_ringbuf();
    uint8_t rx;
    uint8_t c = 0;
    char input[PARSER_CNT];

    memset(input, 0, PARSER_CNT);

    // read the entire ringbuffer
    while (ringbuf_get(rbr, &rx)) {
        if (c < PARSER_CNT-1) {
            input[c] = rx;
        }
        c++;
    }
#else
    char *input = uart_bcl_get_rx_buf();
#endif
    char f = input[0];
    char itoa_buf[CONV_BASE_10_BUF_SZ];
    uint32_t in=0;
    int32_t si=0;
    uint8_t i;
    int8_t j;

    if (f == '?') {
        display_menu();
    } else if (f == 'd') {
        if (str_to_int32(input, &si, 1, strlen(input) - 1, -2147483648, 2147483647) == EXIT_FAILURE) {
            uart_bcl_print(err_conv_str);
        }
        uart_bcl_print(received_str);
        uart_bcl_print(_itoa(itoa_buf, si));
        uart_bcl_print("\r\n");
    } else if (f == 'h') {
        if (str_to_uint32(input, &in, 1, strlen(input) - 1, 0, -1) == EXIT_FAILURE) {
            uart_bcl_print(err_conv_str);
        }
        uart_bcl_print(received_str);
        uart_bcl_print(_utoh(itoa_buf, in));
        uart_bcl_print("\r\n");
    } else if (f == 'a') {
        uart_bcl_print("lotsa text\r\n");
        for (j=20; j>0; j--) {
            for (i=0x30; i<0x40; i++) {
                uart_bcl_tx(i);
            }
            uart_bcl_print("\r\ntest sending a long string abcdefghijklmnopqrs\r\n");
        }
    } else if (f == 't') {
        display_test();
    } else {
        //uart_bcl_tx_str("\r\n", 2);
    }
}
