
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "glue.h"
#include "rtca_now.h"
#include "version.h"
#include "ui.h"


static const char menu_str[]="\
 available commands:\r\n\r\n\
 \e[33;1m?\e[0m - show menu\r\n\
 \e[33;1mr\e[0m  - read current time\r\n\
 \e[33;1mw\e[0m  - write current time\r\n\
 \e[33;1mt\e[0m  - read temperature\r\n";

void display_menu(void)
{
    display_version();
    uart0_print(menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart0_print("ds3231 b");
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
    struct ts t;
    char sconv[CONV_BASE_10_BUF_SZ];
    float temperature;
    int16_t temperature_i16;

    if (f == '?') {
        display_menu();
    } else if (f == 'r') {

        DS3231_get(EUSCI_BASE_ADDR, &t);

        uart0_print(_utoa(sconv, t.year));
        uart0_print(".");
        uart0_print(prepend_padding(sconv, _utoa(sconv, t.mon), PAD_ZEROES, 2));
        uart0_print(".");
        uart0_print(prepend_padding(sconv, _utoa(sconv, t.mday), PAD_ZEROES, 2));
        uart0_print(" ");
        uart0_print(prepend_padding(sconv, _utoa(sconv, t.hour), PAD_ZEROES, 2));
        uart0_print(":");
        uart0_print(prepend_padding(sconv, _utoa(sconv, t.min), PAD_ZEROES, 2));
        uart0_print(":");
        uart0_print(prepend_padding(sconv, _utoa(sconv, t.sec), PAD_ZEROES, 2));

        // there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
        uart0_print("  ");
        uart0_print(_utoa(sconv, t.unixtime));
#endif
        uart0_print("\r\n");
    } else if (f == 'w') {
        t.sec = 0;
        t.min = COMPILE_MIN;
        t.hour = COMPILE_HOUR;
        t.wday = COMPILE_DOW;
        t.mday = COMPILE_DAY;
        t.mon = COMPILE_MON;
        t.year = COMPILE_YEAR;
        t.yday = 0;
        t.isdst = 0;
        t.year_s = 0;
#ifdef CONFIG_UNIXTIME
        t.unixtime = 0;
#endif
        DS3231_set(EUSCI_BASE_ADDR, t);
    } else if (f == 't') {
        //DS3231_get_treg();
        DS3231_get_treg(EUSCI_BASE_ADDR, &temperature);
        temperature *= 10.0;
        temperature_i16 = (int16_t) temperature; 
        uart0_print("temp ");
        if (temperature < 0) {
            uart0_print("-");
        }
        uart0_print(_utoa(sconv, abs(temperature_i16/10)));
        uart0_print(".");
        uart0_print(_utoa(sconv, abs(temperature_i16%10)));
        uart0_print("\r\n");
    }
}
