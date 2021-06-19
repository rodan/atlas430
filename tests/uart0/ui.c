
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "glue.h"
#include "ui.h"

static const char menu_str[]="\
\r\n uart0 and helper module test suite --- available commands:\r\n\r\n\
\e[33;1m?\e[0m  - show menu\r\n\
\e[33;1md\e[0m  - convert input into decimal\r\n\
\e[33;1mh\e[0m  - convert input into hex\r\n";
static const char err_conv_str[]="error during str_to_int32()\r\n";
static const char received_str[]="received ";

void display_menu(void)
{
    uart0_print(menu_str);
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
        uart0_print("123456789\r\n");
    } else {
        //uart0_tx_str("\r\n", 2);
    }
}
