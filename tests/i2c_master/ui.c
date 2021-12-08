
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "glue.h"
#include "version.h"
#include "uart_mapping.h"
#include "rtca_now.h"
#include "ui.h"

#define TEST_CYPRESS_FM24
#define TEST_HSC_SSC
#define TEST_DS3231
#define TEST_TCA6408

static const char menu_str[]="\
 available commands:\r\n\r\n\
 \e[33;1m?\e[0m - show menu\r\n";

#ifdef TEST_CYPRESS_FM24
static const char menu_CYPRESS_FM24[]="\
 \e[33;1mt\e[0m - CYP_FM24 memtest\r\n\
 \e[33;1mr\e[0m - CYP_FM24 tiny read test\r\n\
 \e[33;1mw\e[0m - CYP_FM24 tiny write test\r\n\
 \e[33;1mh\e[0m - CYP_FM24 hex dump of FRAM segment\r\n";
#endif

#ifdef TEST_HSC_SSC
static const char menu_HSC_SSC[]="\
 \e[33;1m1\e[0m - HSC_SSC read sensor\r\n";
#endif

#ifdef TEST_DS3231
static const char menu_DS3231[]="\
 \e[33;1m2\e[0m - DS3231 read\r\n\
 \e[33;1m3\e[0m - DS3231 write\r\n\
 \e[33;1m4\e[0m - DS3231 read temp\r\n";
#endif

#ifdef TEST_TCA6408
static const char menu_TCA6408[]="\
 \e[33;1m5\e[0m - TCA6308 read\r\n\
 \e[33;1m6\e[0m - TCA6408 write\r\n";
#endif

#ifdef TEST_CYPRESS_FM24
void display_memtest(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint32_t start_addr, const uint32_t stop_addr, FM24_test_t test)
{
    uint32_t el;
    uint32_t rows_tested;
    char itoa_buf[CONV_BASE_10_BUF_SZ];

    uart_bcl_print(" \e[36;1m*\e[0m testing ");
    uart_bcl_print(_utoh32(itoa_buf, start_addr));
    uart_bcl_print(" - ");
    uart_bcl_print(_utoh32(itoa_buf, stop_addr));
    uart_bcl_print(" with pattern #");
    uart_bcl_print(_utoa(itoa_buf, test));
    uart_bcl_print("    ");

    el = FM24_memtest(usci_base_addr, slave_addr, start_addr, stop_addr, test, &rows_tested);

    uart_bcl_print(_utoa(itoa_buf, rows_tested * 8));
    if (el == 0) { 
        uart_bcl_print(" bytes tested \e[32;1mok\e[0m\r\n");
    } else {
        uart_bcl_print(" bytes tested with \e[31;1m");
        uart_bcl_print(_utoa(itoa_buf, el));
        uart_bcl_print(" errors\e[0m\r\n");
    }
}

void print_buf_fram(const uint32_t address, const uint32_t size)
{
    uint32_t bytes_remaining = size;
    uint32_t bytes_to_be_printed, bytes_printed = 0;
    char itoa_buf[CONV_BASE_10_BUF_SZ];
    uint16_t i;
    uint8_t *read_ptr = NULL;
    uint8_t row[16];

    while (bytes_remaining > 0) {
    
        if (bytes_remaining > 16) {
            bytes_to_be_printed = 16;
        } else {
            bytes_to_be_printed = bytes_remaining;
        }

        uart_bcl_print(_utoh32(itoa_buf, bytes_printed));
        uart_bcl_print(": ");

        FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, row, address + bytes_printed, bytes_to_be_printed);
        read_ptr = &row[0];

        for (i = 0; i < bytes_to_be_printed; i++) {
            uart_bcl_print(_utoh8(itoa_buf, *read_ptr++));
            if (i & 0x1) {
                uart_bcl_print(" ");
            }
        }
        uart_bcl_print(" ");

        read_ptr = &row[0];
        for (i = 0; i < bytes_to_be_printed; i++) {
            uart_bcl_tx_str((char *)read_ptr++, 1);
        }

        uart_bcl_print("\r\n");
        bytes_printed += bytes_to_be_printed;
        bytes_remaining -= bytes_to_be_printed;
    }
}
#endif

void display_menu(void)
{
    display_version();
    uart_bcl_print(menu_str);
#ifdef TEST_CYPRESS_FM24
    uart_bcl_print(menu_CYPRESS_FM24);
#endif
#ifdef TEST_HSC_SSC
    uart_bcl_print(menu_HSC_SSC);
#endif
#ifdef TEST_DS3231
    uart_bcl_print(menu_DS3231);
#endif
#ifdef TEST_TCA6408
    uart_bcl_print(menu_TCA6408);
#endif
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_bcl_print("i2c master b");
    uart_bcl_print(_utoa(sconv, BUILD));
    uart_bcl_print("\r\n");
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

#ifdef TEST_CYPRESS_FM24
    uint8_t data_r[8]; // test 8 bytes (1 row) at a time
    //uint8_t foo[9]="hello wo";
    uint8_t bar[9]="33333333";
#endif

#ifdef TEST_HSC_SSC
    struct HSC_SSC_pkt ps;
    uint32_t hsc_pressure;
    int16_t hsc_temperature;
#endif

#ifdef TEST_DS3231
    struct ts t;
    float ds3231_temperature;
    int16_t ds3231_temperature_i16;
#endif

#ifdef TEST_TCA6408
    uint8_t tca6408_reg;
#endif

#if defined TEST_HSC_SSC || defined TEST_DS3231 || defined TEST_TCA6408
    char sconv[CONV_BASE_10_BUF_SZ];
#endif

    if (f == '?') {
        display_menu();

#ifdef TEST_CYPRESS_FM24
    } else if (f == 'w') {
        //FM24_write(I2C_BASE_ADDR, FM24_SLAVE_ADDR, foo, FM_LA-20, 8);
        FM24_write(I2C_BASE_ADDR, FM24_SLAVE_ADDR, bar, 0, 8);
    //} else if (f == 'b') {
        //FM24_write(I2C_BASE_ADDR, FM24_SLAVE_ADDR, bar, 0x90, 8);
    } else if (f == 'r') {
        //FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, data_r, FM_LA-20, 8);
        FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, data_r, 0, 8);
    } else if (f == 't') {
        //display_memtest(I2C_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(I2C_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(I2C_BASE_ADDR, 0x10, 0xb0, TEST_AA);
        //display_memtest(I2C_BASE_ADDR, 0x40, 0x60, TEST_FF);
        //display_memtest(I2C_BASE_ADDR, 0x60, 0x80, TEST_AA);
        //display_memtest(I2C_BASE_ADDR, 0x90, 0x98, TEST_00);

        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_FF);
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_CNT);
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_00);
        uart_bcl_print(" * roll over test\r\n");
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, FM_LA - 3, FM_LA + 5, TEST_CNT);
    //} else if (f == 'r') {
    //    FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, data_r, FM_LA-3, 8);
    //} else if (f == 'w') {
    //    display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, FM_LA - 3, FM_LA + 5, TEST_CNT);
    } else if (f == 'h') {
        print_buf_fram(FM_LA - 63, 128);
#endif

#ifdef TEST_HSC_SSC
    } else if (f == '1') {
        HSC_SSC_read(I2C_BASE_ADDR, HSC_SLAVE_ADDR, &ps);
        HSC_SSC_convert(ps, &hsc_pressure, &hsc_temperature, OUTPUT_MIN, OUTPUT_MAX, PRESSURE_MIN,
                   PRESSURE_MAX);

        uart_bcl_print("status: ");
        uart_bcl_print(_utoh(sconv, ps.status));
        uart_bcl_print("\r\nbridge_data: ");
        uart_bcl_print(_utoh(sconv, ps.bridge_data));
        uart_bcl_print("\r\ntemperature_data: ");
        uart_bcl_print(_utoh(sconv, ps.temperature_data));
        uart_bcl_print("\r\npressure: ");
        uart_bcl_print(_utoa(sconv, hsc_pressure));
        uart_bcl_print("\r\ntemperature: ");
        if ( hsc_temperature<0 ) {
            uart_bcl_print("-");
        }
        uart_bcl_print(_utoa(sconv, abs(hsc_temperature / 100)));
        uart_bcl_print(".");
        uart_bcl_print(prepend_padding(sconv, _utoa(sconv, abs(hsc_temperature % 100)), PAD_ZEROES, 2));
        uart_bcl_print("\r\n");
#endif

#ifdef TEST_DS3231
    } else if (f == '2') {

        DS3231_get(I2C_BASE_ADDR, &t);

        uart_bcl_print(_utoa(sconv, t.year));
        uart_bcl_print(".");
        uart_bcl_print(prepend_padding(sconv, _utoa(sconv, t.mon), PAD_ZEROES, 2));
        uart_bcl_print(".");
        uart_bcl_print(prepend_padding(sconv, _utoa(sconv, t.mday), PAD_ZEROES, 2));
        uart_bcl_print(" ");
        uart_bcl_print(prepend_padding(sconv, _utoa(sconv, t.hour), PAD_ZEROES, 2));
        uart_bcl_print(":");
        uart_bcl_print(prepend_padding(sconv, _utoa(sconv, t.min), PAD_ZEROES, 2));
        uart_bcl_print(":");
        uart_bcl_print(prepend_padding(sconv, _utoa(sconv, t.sec), PAD_ZEROES, 2));

        // there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
        uart_bcl_print("  ");
        uart_bcl_print(_utoa(sconv, t.unixtime));
#endif
        uart_bcl_print("\r\n");
    } else if (f == '3') {
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
        DS3231_set(I2C_BASE_ADDR, t);
    } else if (f == '4') {
        //DS3231_get_treg();
        DS3231_get_treg(I2C_BASE_ADDR, &ds3231_temperature);
        ds3231_temperature *= 10.0;
        ds3231_temperature_i16 = (int16_t) ds3231_temperature; 
        uart_bcl_print("temp ");
        if (ds3231_temperature < 0) {
            uart_bcl_print("-");
        }
        uart_bcl_print(_utoa(sconv, abs(ds3231_temperature_i16/10)));
        uart_bcl_print(".");
        uart_bcl_print(_utoa(sconv, abs(ds3231_temperature_i16%10)));
        uart_bcl_print("\r\n");
#endif

#ifdef TEST_TCA6408
    } else if (f == '5') {
        TCA6408_read(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_INPUT);
        uart_bcl_print("input   ");
        uart_bcl_print(_utoh(sconv, tca6408_reg));

        TCA6408_read(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_OUTPUT);
        uart_bcl_print("\r\noutput   ");
        uart_bcl_print(_utoh(sconv, tca6408_reg));

        TCA6408_read(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_POL_INV);
        uart_bcl_print("\r\npol inv  ");
        uart_bcl_print(_utoh(sconv, tca6408_reg));

        TCA6408_read(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_CONF);
        uart_bcl_print("\r\nconf     ");
        uart_bcl_print(_utoh(sconv, tca6408_reg));

        uart_bcl_print("\r\n");
    } else if (f == '6') {
        tca6408_reg = 0xa;
        TCA6408_write(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_CONF);
#endif
    } 
}
