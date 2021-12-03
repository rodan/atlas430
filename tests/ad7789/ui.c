
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "version.h"
#include "ui.h"

extern spi_descriptor spid_ad7789;
static const char menu_str[]= "\
 available commands:\r\n\r\n\
\e[33;1m?\e[0m  - show menu\r\n\
\e[33;1mi\e[0m  - get reg settings\r\n\
\e[33;1mc\e[0m  - get conversion\r\n";

void display_menu(void)
{
    display_version();
    uart0_print(menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart0_print("ad7789 b");
    uart0_print(_utoa(sconv, BUILD));
    uart0_print("\r\n");
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
    uint32_t conv = 0;
    uint8_t ret;
    uint8_t xfer;

    if (f == '?') {
        display_menu();
    } else if (f == 'r') {
        ret = AD7789_rst(&spid_ad7789);
        uart0_print("ad7789 rst ");
        uart0_print(_utoh(itoa_buf, ret));
        uart0_print("\r\n");
    } else if (f == 'c') {
        ret = AD7789_get_conv(&spid_ad7789, &conv);
        uart0_print("ad7789 ");
        if (ret == 0) {
            uart0_print("conv ");
            uart0_print(_utoh(itoa_buf, conv));
        } else {
            uart0_print("err ");
            uart0_print(_utoh(itoa_buf, ret));
        }
        uart0_print("\r\n");
    } else if (f == 'i') {
        ret = AD7789_get_status(&spid_ad7789, &xfer);
        uart0_print("ad7789 ");
        if (ret == 0) {
            uart0_print("conv ");
            uart0_print(_utoh(itoa_buf, xfer));
        } else {
            uart0_print("err ");
            uart0_print(_utoh(itoa_buf, ret));
        }
        uart0_print("\r\n");
    }
}
