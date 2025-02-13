
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "atlas430.h"
#include "version.h"
#include "rtca_now.h"
#include "ui.h"

//#define TEST_CYPRESS_FM24
//#define TEST_HSC_SSC
#define TEST_HBMPS
//#define TEST_DS3231
//#define TEST_TCA6408

extern uart_descriptor bc;

static const char menu_str[]="\
 available commands:\r\n\r\n\
 \033[33;1m?\033[0m - show menu\r\n";

#ifdef TEST_CYPRESS_FM24
static const char menu_CYPRESS_FM24[]="\
 \033[33;1mt\033[0m - CYP_FM24 memtest\r\n\
 \033[33;1mr\033[0m - CYP_FM24 tiny read test\r\n\
 \033[33;1mw\033[0m - CYP_FM24 tiny write test\r\n\
 \033[33;1mh\033[0m - CYP_FM24 hex dump of FRAM segment\r\n";
#endif

#ifdef TEST_HSC_SSC
static const char menu_HSC_SSC[]="\
 \033[33;1m1\033[0m - HSC_SSC read sensor\r\n";
#endif

#ifdef TEST_DS3231
static const char menu_DS3231[]="\
 \033[33;1m2\033[0m - DS3231 read\r\n\
 \033[33;1m3\033[0m - DS3231 write\r\n\
 \033[33;1m4\033[0m - DS3231 read temp\r\n";
#endif

#ifdef TEST_TCA6408
static const char menu_TCA6408[]="\
 \033[33;1m5\033[0m - TCA6308 read\r\n\
 \033[33;1m6\033[0m - TCA6408 write\r\n";
#endif

#ifdef TEST_HBMPS
static const char menu_HBMPS[]="\
 \033[33;1m7\033[0m - HBMPS read sensor\r\n";
#endif

#ifdef TEST_CYPRESS_FM24
void display_memtest(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint32_t start_addr, const uint32_t stop_addr, FM24_test_t test)
{
    uint32_t el;
    uint32_t rows_tested;
    char itoa_buf[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, " \033[36;1m*\033[0m testing ");
    uart_print(&bc, _utoh32(itoa_buf, start_addr));
    uart_print(&bc, " - ");
    uart_print(&bc, _utoh32(itoa_buf, stop_addr));
    uart_print(&bc, " with pattern #");
    uart_print(&bc, _utoa(itoa_buf, test));
    uart_print(&bc, "    ");

    el = FM24_memtest(usci_base_addr, slave_addr, start_addr, stop_addr, test, &rows_tested);

    uart_print(&bc, _utoa(itoa_buf, rows_tested * 8));
    if (el == 0) { 
        uart_print(&bc, " bytes tested \033[32;1mok\033[0m\r\n");
    } else {
        uart_print(&bc, " bytes tested with \033[31;1m");
        uart_print(&bc, _utoa(itoa_buf, el));
        uart_print(&bc, " errors\033[0m\r\n");
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

        uart_print(&bc, _utoh32(itoa_buf, bytes_printed));
        uart_print(&bc, ": ");

        FM24_read(I2C_BASE_ADDR, FM24_SLAVE_ADDR, row, address + bytes_printed, bytes_to_be_printed);
        read_ptr = &row[0];

        for (i = 0; i < bytes_to_be_printed; i++) {
            uart_print(&bc, _utoh8(itoa_buf, *read_ptr++));
            if (i & 0x1) {
                uart_print(&bc, " ");
            }
        }
        uart_print(&bc, " ");

        read_ptr = &row[0];
        for (i = 0; i < bytes_to_be_printed; i++) {
            uart_tx_str(&bc, (char *)read_ptr++, 1);
        }

        uart_print(&bc, "\r\n");
        bytes_printed += bytes_to_be_printed;
        bytes_remaining -= bytes_to_be_printed;
    }
}
#endif

void display_menu(void)
{
    display_version();
    uart_print(&bc, menu_str);
#ifdef TEST_CYPRESS_FM24
    uart_print(&bc, menu_CYPRESS_FM24);
#endif
#ifdef TEST_HSC_SSC
    uart_print(&bc, menu_HSC_SSC);
#endif
#ifdef TEST_DS3231
    uart_print(&bc, menu_DS3231);
#endif
#ifdef TEST_TCA6408
    uart_print(&bc, menu_TCA6408);
#endif
#ifdef TEST_HBMPS
    uart_print(&bc, menu_HBMPS);
#endif

}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, "i2c master b");
    uart_print(&bc, _utoa(sconv, BUILD));
    uart_print(&bc, "\r\n");
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

#ifdef TEST_HBMPS
    device_t hbmps;
    bus_desc_i2c_hw_master_t hbmps_bus_desc;
    hbmps_spec_t hbmps_spec;
    uint8_t hbmps_buff[HBMPS_BUFF_SIZE];
    struct hbmps_pkt hbmps_raw;
    int32_t hbmps_pressure;
    int32_t hbmps_temperature;

    hbmps_spec.output_min = 0x666;
    hbmps_spec.output_max = 0x399a;
    hbmps_spec.pressure_min = 0;
    hbmps_spec.pressure_max = 206843;

    bus_init_i2c_hw_master(&hbmps, I2C_BASE_ADDR, HSC_SLAVE_ADDR, &hbmps_bus_desc, hbmps_buff, HBMPS_BUFF_SIZE);
#endif

#ifdef TEST_DS3231
    struct ts t;
    float ds3231_temperature;
    int16_t ds3231_temperature_i16;
#endif

#ifdef TEST_TCA6408
    uint8_t tca6408_reg;
#endif

#if defined TEST_HSC_SSC || defined TEST_HBMPS || defined TEST_DS3231 || defined TEST_TCA6408
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
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_00);
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_CNT);
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, 0, FM_LA, TEST_AA);
        uart_print(&bc, " * roll over test\r\n");
        display_memtest(I2C_BASE_ADDR, FM24_SLAVE_ADDR, FM_LA - 3, FM_LA + 5, TEST_CNT);
    } else if (f == 'h') {
        print_buf_fram(FM_LA - 63, 128);
#endif

#ifdef TEST_HSC_SSC
    } else if (f == '1') {
        HSC_SSC_read(I2C_BASE_ADDR, HSC_SLAVE_ADDR, &ps);
        HSC_SSC_convert(ps, &hsc_pressure, &hsc_temperature, OUTPUT_MIN, OUTPUT_MAX, PRESSURE_MIN,
                   PRESSURE_MAX);

        uart_print(&bc, "status: ");
        uart_print(&bc, _utoh(sconv, ps.status));
        uart_print(&bc, "\r\nbridge_data: ");
        uart_print(&bc, _utoh(sconv, ps.bridge_data));
        uart_print(&bc, "\r\ntemperature_data: ");
        uart_print(&bc, _utoh(sconv, ps.temperature_data));
        uart_print(&bc, "\r\npressure: ");
        uart_print(&bc, _utoa(sconv, hsc_pressure));
        uart_print(&bc, "\r\ntemperature: ");
        if ( hsc_temperature<0 ) {
            uart_print(&bc, "-");
        }
        uart_print(&bc, _utoa(sconv, abs(hsc_temperature / 100)));
        uart_print(&bc, ".");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, abs(hsc_temperature % 100)), PAD_ZEROES, 2));
        uart_print(&bc, "\r\n");
#endif

#ifdef TEST_HBMPS
    } else if (f == '7') {

        hbmps_read(&hbmps, &hbmps_raw);
        hbmps_convert(&hbmps_raw, &hbmps_pressure, &hbmps_temperature, &hbmps_spec);

        uart_print(&bc, "status: ");
        uart_print(&bc, _utoh(sconv, hbmps_raw.status));
        uart_print(&bc, "\r\nbridge_data: ");
        uart_print(&bc, _utoh(sconv, hbmps_raw.bridge_data));
        uart_print(&bc, "\r\ntemperature_data: ");
        uart_print(&bc, _utoh(sconv, hbmps_raw.temperature_data));
        uart_print(&bc, "\r\npressure: ");
        if ( hbmps_pressure<0 ) {
            uart_print(&bc, "-");
        }
        uart_print(&bc, _utoa(sconv, hbmps_pressure));
        uart_print(&bc, "\r\ntemperature: ");
        if ( hbmps_temperature<0 ) {
            uart_print(&bc, "-");
        }
        uart_print(&bc, _utoa(sconv, abs(hbmps_temperature / 100)));
        uart_print(&bc, ".");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, abs(hbmps_temperature % 100)), PAD_ZEROES, 2));
        uart_print(&bc, "\r\n");
#endif

#ifdef TEST_DS3231
    } else if (f == '2') {

        DS3231_get(I2C_BASE_ADDR, &t);

        uart_print(&bc, _utoa(sconv, t.year));
        uart_print(&bc, ".");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.mon), PAD_ZEROES, 2));
        uart_print(&bc, ".");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.mday), PAD_ZEROES, 2));
        uart_print(&bc, " ");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.hour), PAD_ZEROES, 2));
        uart_print(&bc, ":");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.min), PAD_ZEROES, 2));
        uart_print(&bc, ":");
        uart_print(&bc, prepend_padding(sconv, _utoa(sconv, t.sec), PAD_ZEROES, 2));

        // there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
        uart_print(&bc, "  ");
        uart_print(&bc, _utoa(sconv, t.unixtime));
#endif
        uart_print(&bc, "\r\n");
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
        uart_print(&bc, "temp ");
        if (ds3231_temperature < 0) {
            uart_print(&bc, "-");
        }
        uart_print(&bc, _utoa(sconv, abs(ds3231_temperature_i16/10)));
        uart_print(&bc, ".");
        uart_print(&bc, _utoa(sconv, abs(ds3231_temperature_i16%10)));
        uart_print(&bc, "\r\n");
#endif

#ifdef TEST_TCA6408
    } else if (f == '5') {
        TCA6408_read(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_INPUT);
        uart_print(&bc, "input   ");
        uart_print(&bc, _utoh(sconv, tca6408_reg));

        TCA6408_read(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_OUTPUT);
        uart_print(&bc, "\r\noutput   ");
        uart_print(&bc, _utoh(sconv, tca6408_reg));

        TCA6408_read(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_POL_INV);
        uart_print(&bc, "\r\npol inv  ");
        uart_print(&bc, _utoh(sconv, tca6408_reg));

        TCA6408_read(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_CONF);
        uart_print(&bc, "\r\nconf     ");
        uart_print(&bc, _utoh(sconv, tca6408_reg));

        uart_print(&bc, "\r\n");
    } else if (f == '6') {
        tca6408_reg = 0xa;
        TCA6408_write(I2C_BASE_ADDR, TCA6408_SLAVE_ADDR, &tca6408_reg, TCA6408_CONF);
#endif
    } 
}
