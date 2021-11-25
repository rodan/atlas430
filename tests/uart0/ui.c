
#include <stdio.h>
#include <string.h>
#include "glue.h"
#include "version.h"
#include "ui.h"

//#define TEST_TX_STR
#define TEST_PRINT
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
    uart0_print(menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart0_print("uart0 ");
    uart0_print(_utoa(sconv, COMMIT));
    uart0_print("b");
    uart0_print(_utoa(sconv, BUILD));
    uart0_print("\r\n");
}

void display_test(void)
{
#if defined TEST_ITOA || defined TEST_SNPRINTF || defined TEST_UTOH || defined TEST_UTOB
    char buf[CONV_BASE_10_BUF_SZ];
#endif

#ifdef TEST_TX_STR
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

#ifdef TEST_PRINT
    uart0_print("h1llo world\r\n"); // 13
    uart0_print("he2lo world\r\n"); // 26
    uart0_print("hel3o world\r\n"); // 40
    uart0_print("hell4 world\r\n"); // 56
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
}

#define PARSER_CNT 16

void parse_user_input(void)
{
#ifdef UART0_RX_USES_RINGBUF
    struct ringbuf *rbr = uart0_get_rx_ringbuf();
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
    char *input = uart0_get_rx_buf();
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
            uart0_print(err_conv_str);
        }
        uart0_print(received_str);
        uart0_print(_itoa(itoa_buf, si));
        uart0_print("\r\n");
    } else if (f == 'h') {
        if (str_to_uint32(input, &in, 1, strlen(input) - 1, 0, -1) == EXIT_FAILURE) {
            uart0_print(err_conv_str);
        }
        uart0_print(received_str);
        uart0_print(_utoh(itoa_buf, in));
        uart0_print("\r\n");
    } else if (f == 'a') {
        uart0_print("lotsa text\r\n");
        for (j=20; j>0; j--) {
            for (i=0x30; i<0x40; i++) {
                uart0_tx(i);
            }
            uart0_print("\r\ntest sending a long string abcdefghijklmnopqrs\r\n");
        }
    } else if (f == 't') {
        display_test();
    } else {
        //uart0_tx_str("\r\n", 2);
    }
}
