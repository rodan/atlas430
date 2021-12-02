
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "proj.h"
#include "glue.h"
#include "version.h"
#include "ui.h"

struct HSC_SSC_pkt ps;

#define STR_LEN 64
char str_temp[STR_LEN];

static const char menu_str[]="\
 available commands:\r\n\r\n\
 \e[33;1m?\e[0m  - show menu\r\n\
 \e[33;1mr\e[0m  - read current values\r\n";

void display_menu(void)
{
    display_version();
    uart0_print(menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart0_print("HSC b");
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

    uint32_t pressure;
    int16_t temperature;

    char sconv[CONV_BASE_10_BUF_SZ];


    if (f == '?') {
        display_menu();
    } else if (f == 'r') {       // [r]ead value
        HSC_SSC_read(EUSCI_BASE_ADDR, SLAVE_ADDR, &ps);
        HSC_SSC_convert(ps, &pressure, &temperature, OUTPUT_MIN, OUTPUT_MAX, PRESSURE_MIN,
                   PRESSURE_MAX);

        uart0_print("status: ");
        uart0_print(_utoh(sconv, ps.status));
        uart0_print("\r\nbridge_data: ");
        uart0_print(_utoh(sconv, ps.bridge_data));
        uart0_print("\r\ntemperature_data: ");
        uart0_print(_utoh(sconv, ps.temperature_data));
        uart0_print("\r\npressure: ");
        uart0_print(_utoa(sconv, pressure));
        uart0_print("\r\ntemperature: ");
        if ( temperature<0 ) {
            uart0_print("-");
        }
        uart0_print(_utoa(sconv, abs(temperature / 100)));
        uart0_print(".");
        uart0_print(prepend_padding(sconv, _utoa(sconv, abs(temperature % 100)), PAD_ZEROES, 2));
        uart0_print("\r\n");
    }
}
