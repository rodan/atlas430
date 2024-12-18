
#include <stdio.h>
#include <string.h>
#include "atlas430.h"
#include "version.h"
#include "ui.h"

extern uart_descriptor bc;

static const char menu_str[]="\
 available commands:\r\n\r\n\
\033[33;1m?\033[0m - show menu\r\n\
\033[33;1ms\033[0m - display schedule\r\n";

void display_menu(void)
{
    display_version();
    uart_print(&bc, menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, "sheduler b");
    uart_print(&bc, _utoa(sconv, BUILD));
    uart_print(&bc, "\r\n");
}

void display_schedule(void)
{
    uint8_t c;
    uint32_t trigger;
    uint8_t flag;
    char itoa_buf[CONV_BASE_10_BUF_SZ];

    for (c = 0; c < SCH_SLOT_CNT; c++) {
        sch_get_trigger_slot(c, &trigger, &flag);
        uart_print(&bc, _utoa(itoa_buf, c));
        uart_print(&bc, " \t");
        uart_print(&bc, _utoa(itoa_buf, trigger));
        uart_print(&bc, " \t");
        uart_print(&bc, _utoa(itoa_buf, flag));
        uart_print(&bc, "\r\n");
    }
    trigger = sch_get_trigger_next();
    uart_print(&bc, "sch next ");
    uart_print(&bc, _utoa(itoa_buf, trigger));
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

    if (f == '?') {
        display_menu();
    } else if (f == 's') {
        display_schedule();
    }
}
