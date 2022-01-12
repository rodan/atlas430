
#include <stdio.h>
#include <string.h>
#include "glue.h"
#include "version.h"
#include "rtca_now.h"
#include "ui.h"

extern uart_descriptor bc;
extern spi_descriptor spid_ds3234;
static const uint8_t hello_str[6] = "hello";

static const char menu_str[] = "\
\r\n ds3234 module test suite --- available commands:\r\n\r\n\
 \e[33;1m?\e[0m  - show menu\r\n";

#ifdef CONFIG_DS3234
static const char menu_DS3234[] = "\
 \e[33;1m0\e[0m  - DS3234 status\r\n\
 \e[33;1m1\e[0m  - DS3234 read rtc\r\n\
 \e[33;1m2\e[0m  - DS3234 write rtc\r\n\
 \e[33;1m3\e[0m  - DS3234 sram test\r\n\
 \e[33;1m4\e[0m  - DS3234 alarm1 test\r\n\
 \e[33;1m5\e[0m  - DS3234 alarm2 test\r\n\
 \e[33;1m6\e[0m  - DS3234 clear alarm\r\n\
 \e[33;1m7\e[0m  - DS3234 temperature read\r\n";
#endif

void display_menu(void)
{
    display_version();
    uart_print(&bc, menu_str);
#ifdef CONFIG_DS3234
    uart_print(&bc, menu_DS3234);
#endif
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, "spi master b");
    uart_print(&bc, _utoa(sconv, BUILD));
    uart_print(&bc, "\r\n");
}

/** quick and dirty function to increment the current time by a few minutes [1-59] for alarm testing
    /note not to be used at the end of the month
    @param t input ts struct
    @param min minutes of increment [0-59]
    @return void
*/
void add_some_minutes(struct ts *t, const uint8_t min)
{
    t->min += min;
    if (t->min > 59) {
        t->min -= 60;
        t->hour += 1;
    }

    if (t->hour > 23) {
        t->hour -= 24;
        t->mday += 1;
    }
}

#define PARSER_CNT 16

void parse_user_input(void)
{
#if defined UART_RX_USES_RINGBUF
    struct ringbuf *rbr = uart_get_rx_ringbuf(&bc);
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
    char *input = uart_get_rx_buf(&bc);
#endif
    char f = input[0];
    char itoa_buf[18];
    struct ts rtc;
    int16_t temperature;
    uint8_t reg, reg_wr;
    uint8_t test_str[6];

    if (f == '?') {
        display_menu();

#ifdef CONFIG_DS3234
    } else if (f == '1') {
        memset(&rtc, 0, sizeof(rtc));
        DS3234_read_rtc(&spid_ds3234, &rtc);

        uart_print(&bc, prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.hour), PAD_ZEROES, 2));
        uart_print(&bc, ":");
        uart_print(&bc, prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.min), PAD_ZEROES, 2));
        uart_print(&bc, ":");
        uart_print(&bc, prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.sec), PAD_ZEROES, 2));
        uart_print(&bc, " ");
        uart_print(&bc, _utoa(&itoa_buf[0], rtc.year));
        uart_print(&bc, "/");
        uart_print(&bc, prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.mon), PAD_ZEROES, 2));
        uart_print(&bc, "/");
        uart_print(&bc, prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.mday), PAD_ZEROES, 2));
        uart_print(&bc, "\r\n");
    } else if (f == '2') {
        memset(&rtc, 0, sizeof(rtc));
        rtc.year = COMPILE_YEAR;
        rtc.mon = COMPILE_MON;
        rtc.mday = COMPILE_DAY;
        rtc.wday = COMPILE_DOW;
        rtc.hour = COMPILE_HOUR;
        rtc.min = COMPILE_MIN;
        rtc.sec = 59;
        DS3234_write_rtc(&spid_ds3234, &rtc);
        // clear the OSF flag in order to indicate future oscillator malfunctions
        DS3234_read_reg(&spid_ds3234, DS3234_REG_STA, &reg);
        reg_wr = reg & ~DS3234_OSF;
        if (reg_wr != reg) {
            DS3234_write_reg(&spid_ds3234, DS3234_REG_STA, reg_wr);
        }
    } else if (f == '3') {
        DS3234_read_sram(&spid_ds3234, 0, test_str, 6);
        DS3234_write_sram(&spid_ds3234, 0, (uint8_t *) hello_str, 6);
        DS3234_read_sram(&spid_ds3234, 0, test_str, 6);
    } else if (f == '4') {
        memset(&rtc, 0, sizeof(rtc));
        DS3234_read_rtc(&spid_ds3234, &rtc);
        add_some_minutes(&rtc, 1);
        DS3234_write_a1(&spid_ds3234, &rtc, DS3234_A1_SMHDm_MATCH);
        // enable alarm1
        DS3234_read_reg(&spid_ds3234, DS3234_REG_CTRL, &reg);
        reg_wr = reg | DS3234_A1IE;
        if (reg_wr != reg) {
            DS3234_write_reg(&spid_ds3234, DS3234_REG_CTRL, reg_wr);
        }
    } else if (f == '5') {
        memset(&rtc, 0, sizeof(rtc));
        DS3234_read_rtc(&spid_ds3234, &rtc);
        add_some_minutes(&rtc, 2);
        DS3234_write_a2(&spid_ds3234, &rtc, DS3234_A2_MHDm_MATCH);
        // enable alarm1
        DS3234_read_reg(&spid_ds3234, DS3234_REG_CTRL, &reg);
        reg_wr = reg | DS3234_A2IE;
        if (reg_wr != reg) {
            DS3234_write_reg(&spid_ds3234, DS3234_REG_CTRL, reg_wr);
        }
    } else if (f == '6') {
        // clear the OSF flag in order to indicate future oscillator malfunctions
        // also clear the alarm flags
        DS3234_read_reg(&spid_ds3234, DS3234_REG_STA, &reg);
        reg_wr = reg & ~(DS3234_A1F | DS3234_A2F);
        if (reg_wr != reg) {
            DS3234_write_reg(&spid_ds3234, DS3234_REG_STA, reg_wr);
        }
    } else if (f == '0') {
#if defined __MSP430FR5994__
        uart_print(&bc, "INT pin: ");
        if (P5IN & BIT3) {
            uart_print(&bc, "inactive");
        } else {
            uart_print(&bc, "asserted");
        }
#endif
        uart_print(&bc, "\r\ncontrol reg: ");
        DS3234_read_reg(&spid_ds3234, DS3234_REG_CTRL, &reg);
        uart_print(&bc, _utoh(&itoa_buf[0], reg));
        uart_print(&bc, "\r\nstatus reg: ");
        DS3234_read_reg(&spid_ds3234, DS3234_REG_STA, &reg);
        uart_print(&bc, _utoh(&itoa_buf[0], reg));
        uart_print(&bc, "\r\n");
    } else if (f == '7') {
        DS3234_read_temp(&spid_ds3234, &temperature);
        uart_print(&bc, _itoa(&itoa_buf[0], temperature));
        uart_print(&bc, "\r\n");
#endif

    } else {
        uart_print(&bc, "\r\n");
    }
}
