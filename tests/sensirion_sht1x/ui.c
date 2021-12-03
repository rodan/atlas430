
#include <stdio.h>
#include <string.h>
#include "glue.h"
#include "version.h"
#include "ui.h"

#define STR_LEN 64

static const char menu_str[]="\
 available commands:\r\n\r\n\
 \e[33;1m?\e[0m  - show menu\r\n\
 \e[33;1mr\e[0m  - read sensor\r\n";

void display_menu(void)
{
    display_version();
    uart0_print(menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart0_print("sht1x b");
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
        if (c < PARSER_CNT - 1) {
            input[c] = rx;
        }
        c++;
    }
#else
    char *input = uart0_get_rx_buf();
#endif
    char f = input[0];
    char str_temp[STR_LEN];
    uint8_t reg;
    int16_t temperature;
    uint16_t rh;

    if (f == '?') {
        display_menu();
    } else if (f == 'r') {
        SHT1X_get_status(&reg);
        snprintf(str_temp, STR_LEN, "reg: 0x%x\r\n", reg);
        uart0_print(str_temp);

        SHT1X_get_meas(&temperature, &rh);
        snprintf(str_temp, STR_LEN, "temperature: %d.%d degC\r\n", temperature / 100, temperature % 100);
        uart0_print(str_temp);
        snprintf(str_temp, STR_LEN, "humidity: %d.%d %%rh\r\n", rh / 100, rh % 100);
    }
}
