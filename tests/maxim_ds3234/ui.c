
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "glue.h"
#include "rtca_now.h"
#include "ui.h"

extern spi_descriptor spid_ds3234;
static const uint8_t hello_str[6] = "hello";
static const char menu_str[] = "\
\r\n ds3234 module test suite --- available commands:\r\n\r\n\
 \e[33;1m?\e[0m  - show menu\r\n\
 \e[33;1ms\e[0m  - status\r\n\
 \e[33;1mr\e[0m  - read rtc\r\n\
 \e[33;1mw\e[0m  - write rtc\r\n\
 \e[33;1mm\e[0m  - sram test\r\n\
 \e[33;1m1\e[0m  - alarm1 test\r\n\
 \e[33;1m2\e[0m  - alarm2 test\r\n\
 \e[33;1mc\e[0m  - clear alarm\r\n\
 \e[33;1mt\e[0m  - temperature read\r\n";

void display_menu(void)
{
    uart0_print(menu_str);
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

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char itoa_buf[18];
    struct ts rtc;
    int16_t temperature;
    uint8_t reg, reg_wr;
    uint8_t test_str[6];

    if (f == '?') {
        display_menu();
    } else if (f == 'r') {
        memset(&rtc, 0, sizeof(rtc));
        DS3234_read_rtc(&spid_ds3234, &rtc);

        uart0_print(prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.hour), PAD_ZEROES, 2));
        uart0_print(":");
        uart0_print(prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.min), PAD_ZEROES, 2));
        uart0_print(":");
        uart0_print(prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.sec), PAD_ZEROES, 2));
        uart0_print(" ");
        uart0_print(_utoa(&itoa_buf[0], rtc.year));
        uart0_print("/");
        uart0_print(prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.mon), PAD_ZEROES, 2));
        uart0_print("/");
        uart0_print(prepend_padding(itoa_buf, _utoa(&itoa_buf[0], rtc.mday), PAD_ZEROES, 2));
        uart0_print("\r\n");
    } else if (f == 'w') {
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
    } else if (f == 'm') {
        DS3234_read_sram(&spid_ds3234, 0, test_str, 6);
        DS3234_write_sram(&spid_ds3234, 0, (uint8_t *) hello_str, 6);
        DS3234_read_sram(&spid_ds3234, 0, test_str, 6);
    } else if (f == '1') {
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
    } else if (f == '2') {
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
    } else if (f == 'c') {
        // clear the OSF flag in order to indicate future oscillator malfunctions
        // also clear the alarm flags
        DS3234_read_reg(&spid_ds3234, DS3234_REG_STA, &reg);
        reg_wr = reg & ~(DS3234_A1F | DS3234_A2F);
        if (reg_wr != reg) {
            DS3234_write_reg(&spid_ds3234, DS3234_REG_STA, reg_wr);
        }
    } else if (f == 's') {
        uart0_print("INT pin: ");
        if (P5IN & BIT3) {
            uart0_print("inactive");
        } else {
            uart0_print("asserted");
        }
        uart0_print("\r\ncontrol reg: ");
        DS3234_read_reg(&spid_ds3234, DS3234_REG_CTRL, &reg);
        uart0_print(_utoh(&itoa_buf[0], reg));
        uart0_print("\r\nstatus reg: ");
        DS3234_read_reg(&spid_ds3234, DS3234_REG_STA, &reg);
        uart0_print(_utoh(&itoa_buf[0], reg));
        uart0_print("\r\n");
    } else if (f == 't') {
        DS3234_read_temp(&spid_ds3234, &temperature);
        uart0_print(_itoa(&itoa_buf[0], temperature));
        uart0_print("\r\n");
    } else {
        uart0_print("\r\n");
    }
}
