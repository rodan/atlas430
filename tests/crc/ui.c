
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "glue.h"
#include "ui.h"
#include "zcrc.h"

#define STR_LEN 64

static const uint8_t crc_test[] = {
    0x31, 0x32, 0x33, 0x34, 0x35
};

static const char menu_str[]="\
\r\n crc test suite --- available commands:\r\n\r\n\
 \e[33;1m?\e[0m  - show menu\r\n\
 \e[33;1mt\e[0m  - crc test\r\n";

void display_menu(void)
{
    uart0_print(menu_str);
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
    char itoa_buf[CONV_BASE_10_BUF_SZ];
    uint32_t crc32_res1 = 0, crc32_res2 = 0;
    uint16_t crc16_res1 = 0, crc16_res2 = 0;
    uint16_t i;

    if (f == '?') {
        display_menu();
    } else if (f == 't') {

        // crc16 test 1
        crc16_res1 = crc16(crc_test, sizeof(crc_test), 0);

        // crc16 test 2
        crc16bs_start(0x0);
        for(i = 0; i < sizeof(crc_test); i++) {
            crc16bs_upd(crc_test[i]);
        }
        crc16_res2 = crc16bs_end();

        // crc32 test 1
        crc32_res1 = crc32(crc_test, sizeof(crc_test), 0);

        // crc32 test 2
        crc32bs_start(0x0);
        for(i = 0; i < sizeof(crc_test); i++) {
            crc32bs_upd(crc_test[i]);
        }
        crc32_res2 = crc32bs_end();

        uart0_print("test results:\r\n");
        uart0_print("crc16 t1 ");
        uart0_print(_utoh(itoa_buf, crc16_res1));
        uart0_print("\r\n");

        uart0_print("crc16 t2 ");
        uart0_print(_utoh(itoa_buf, crc16_res2));
        uart0_print("\r\n");

        uart0_print("crc32 t1 ");
        uart0_print(_utoh(itoa_buf, crc32_res1));
        uart0_print("\r\n");

        uart0_print("crc32 t2 ");
        uart0_print(_utoh(itoa_buf, crc32_res2));
        uart0_print("\r\n");
    }
}

