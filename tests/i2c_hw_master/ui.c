
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "atlas430.h"
#include "version.h"
#include "rtca_now.h"
#include "ui.h"

extern uart_descriptor bc;

#ifdef TEST_HBMPS
    extern device_t dev_hbmps;
    extern hbmps_spec_t hbmps_spec;
#endif

#ifdef TEST_DSRTC
    extern device_t dev_dsrtc;
#endif

#ifdef TEST_CYPRESS_FM24
    extern device_t dev_fm24;
#endif

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

#ifdef TEST_DSRTC
static const char menu_DSRTC[]="\
 \033[33;1mq\033[0m - dsrtc read\r\n\
 \033[33;1mQ\033[0m - dsrtc write\r\n\
 \033[33;1ma\033[0m - dsrtc read temp\r\n";
#endif

#ifdef TEST_TCA6408
static const char menu_TCA6408[]="\
 \033[33;1m5\033[0m - TCA6308 read\r\n\
 \033[33;1m6\033[0m - TCA6408 write\r\n";
#endif

#ifdef TEST_HBMPS
static const char menu_HBMPS[]="\
 \033[33;1mp\033[0m - hbmps read sensor\r\n";
#endif

#ifdef TEST_CYPRESS_FM24
void display_memtest(device_t *dev, const uint32_t start_addr, const uint32_t stop_addr, FM24_test_t test)
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

    el = fm24_memtest(dev, start_addr, stop_addr, test, &rows_tested);

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

        fm24_read(&dev_fm24, address + bytes_printed, row, bytes_to_be_printed);
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
#ifdef TEST_DSRTC
    uart_print(&bc, menu_DSRTC);
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

    uart_print(&bc, "i2c hw master b");
    uart_print(&bc, _utoa(sconv, BUILD));
    uart_print(&bc, "\r\n");
}

void display_err(const uint16_t rv)
{
    char sconv[CONV_BASE_10_BUF_SZ];
    uart_print(&bc, "\033[31;01merror\033[0m ");
    uart_print(&bc, _utoh(sconv, rv));
    uart_print(&bc, "\n");
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
    uint16_t rv;

#ifdef TEST_CYPRESS_FM24
    uint8_t data_r[8]; // test 8 bytes (1 row) at a time
    //uint8_t foo[9]="hello wo";
    uint8_t bar[9]="33333333";
#endif

#ifdef TEST_HBMPS
    struct hbmps_pkt hbmps_raw;
    int32_t hbmps_pressure;
    int32_t hbmps_temperature;
#endif

#ifdef TEST_DSRTC
    struct ts t;
    int16_t dsrtc_temperature;
#endif

#ifdef TEST_TCA6408
    uint8_t tca6408_reg;
#endif

#if defined TEST_HBMPS || defined TEST_DSRTC || defined TEST_TCA6408
    char sconv[CONV_BASE_10_BUF_SZ];
#endif

    if (f == '?') {
        display_menu();

#ifdef TEST_CYPRESS_FM24
    } else if (f == 'w') {
        //fm24_write(&dev_fm24, FM_LA-20, foo, 8);
        rv = fm24_write(&dev_fm24, 0, bar, 8);
        if (rv != BUS_OK) {
            display_err(rv);
            return;
        }

    //} else if (f == 'b') {
        //fm24_write(&dev_fm24, 0x90, bar, 8);
    } else if (f == 'r') {
        //fm24_read(&dev_fm24, FM_LA-20, data_r, 8);
        rv = fm24_read(&dev_fm24, 0, data_r, 8);
        if (rv != BUS_OK) {
            display_err(rv);
            return;
        }
    } else if (f == 't') {
        //display_memtest(I2C_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(I2C_BASE_ADDR, 0xffe0, FM_LA, TEST_00);
        //display_memtest(I2C_BASE_ADDR, 0x10, 0xb0, TEST_AA);
        //display_memtest(I2C_BASE_ADDR, 0x40, 0x60, TEST_FF);
        //display_memtest(I2C_BASE_ADDR, 0x60, 0x80, TEST_AA);
        //display_memtest(I2C_BASE_ADDR, 0x90, 0x98, TEST_00);

        display_memtest(&dev_fm24, 0, FM_LA, TEST_FF);
        display_memtest(&dev_fm24, 0, FM_LA, TEST_00);
        display_memtest(&dev_fm24, 0, FM_LA, TEST_CNT);
        display_memtest(&dev_fm24, 0, FM_LA, TEST_AA);
        uart_print(&bc, " * roll over test\r\n");
        display_memtest(&dev_fm24, FM_LA - 3, FM_LA + 5, TEST_CNT);
    } else if (f == 'h') {
        print_buf_fram(FM_LA - 63, 128);
#endif

#ifdef TEST_HBMPS
    } else if (f == 'p') {

        rv = hbmps_read(&dev_hbmps, &hbmps_raw);
        if (rv != BUS_OK) {
            display_err(rv);
            return;
        }

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

#ifdef TEST_DSRTC
    } else if (f == 'q') {

        rv = dsrtc_read_rtc(&dev_dsrtc, &t);
        if (rv != BUS_OK) {
            display_err(rv);
            return;
        }

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
    } else if (f == 'Q') {
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
        rv = dsrtc_write_rtc(&dev_dsrtc, &t);
        if (rv != BUS_OK) {
            display_err(rv);
            return;
        }
    } else if (f == 'a') {
        rv = dsrtc_read_temp(&dev_dsrtc, &dsrtc_temperature);
        if (rv != BUS_OK) {
            display_err(rv);
            return;
        }

        uart_print(&bc, "temp ");
        uart_print(&bc, _utoh(sconv, dsrtc_temperature));
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
