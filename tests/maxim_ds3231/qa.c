
#include <inttypes.h>
#include "config.h"
#include "glue.h"
#include "rtca_now.h"
#include "version.h"
#include "qa.h"

//#define STR_LEN 64

void display_menu(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];
    
    uart0_print("\r\n Maxim DS3231 test suite v");
    uart0_print(_utoa(sconv, COMMIT));
    uart0_print(".");
    uart0_print(_utoa(sconv, BUILD));
    uart0_print(" test suite\r\n");
    uart0_print(" --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m  - show menu\r\n");
    uart0_print(" \e[33;1mi\e[0m  - display i2c registers\r\n");
    uart0_print(" \e[33;1mr\e[0m  - read current time\r\n");
    uart0_print(" \e[33;1mw\e[0m  - write current time\r\n");
    uart0_print(" \e[33;1mt\e[0m  - read temperature\r\n");
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
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
        t.unixtime = 0;
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

    } else if (f == 'i') {
        uart0_print("P7SEL0 ");
        uart0_print(_utoh(sconv, P7SEL0));
        uart0_print(" P7SEL1 ");
        uart0_print(_utoh(sconv, P7SEL1));
        uart0_print("\r\nUCB2CTLW0 ");
        uart0_print(_utoh(sconv, UCB2CTLW0));
        uart0_print(" UCB2CTLW1 ");
        uart0_print(_utoh(sconv, UCB2CTLW1));
        uart0_print("\r\nUCB2BRW ");
        uart0_print(_utoh(sconv, UCB2BRW));
        uart0_print(" UCB2STATW ");
        uart0_print(_utoh(sconv, UCB2STATW));
        uart0_print("\r\nUCB2TBCNT ");
        uart0_print(_utoh(sconv, UCB2TBCNT));
        uart0_print("\r\nUCB2RXBUF ");
        uart0_print(_utoh(sconv, UCB2RXBUF));
        uart0_print(" UCB2TXBUF ");
        uart0_print(_utoh(sconv, UCB2TXBUF));
        uart0_print("\r\nUCB2I2COA0 ");
        uart0_print(_utoh(sconv, UCB2I2COA0));
        uart0_print(" UCB2ADDRX ");
        uart0_print(_utoh(sconv, UCB2ADDRX));
        uart0_print("\r\nUCB2ADDMASK ");
        uart0_print(_utoh(sconv, UCB2ADDMASK));
        uart0_print(" UCB2I2CSA ");
        uart0_print(_utoh(sconv, UCB2I2CSA));
        uart0_print("\r\nUCB2IE ");
        uart0_print(_utoh(sconv, UCB2IE));
        uart0_print(" UCB2IFG ");
        uart0_print(_utoh(sconv, UCB2IFG));
        uart0_print("\r\nUCB2IV ");
        uart0_print(_utoh(sconv, UCB2IV));
        uart0_print("\r\n");
     
    } else {
        uart0_print("\r\n");
    }
}
