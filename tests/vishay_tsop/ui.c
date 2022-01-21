
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "version.h"
#include "ui.h"
#include "ir_acquire.h"
#include "ir_send.h"

#define STR_LEN 64
extern uart_descriptor bc;
struct ir_tome temp_command;

static const char menu_str[]="\
 available commands:\r\n\r\n\
 \033[33;1m?\033[0m  - show menu\r\n\
 \033[33;1ms\033[0m  - start IR acquisition\r\n\
 \033[33;1mr\033[0m  - replay signal\r\n";

void display_menu(void)
{
    display_version();
    uart_print(&bc, menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, "tsop b");
    uart_print(&bc, _utoa(sconv, BUILD));
    uart_print(&bc, "\r\n");
}

void qa_acquisition_end(void)
{
    uint16_t *data;
    uint16_t size;
    char itoa_buf[18];
    uint16_t i;

    // signal the state machine that the acquiring time has elapsed
    ir_acquire_sm_set_state(IR_ACQUIRE_SM_STOP);
    ir_acquire_sm();

    ir_acquire_get_acquisition(&data, &size);
    uart_print(&bc, "got ");
    uart_print(&bc, _utoa(itoa_buf, size));
    uart_print(&bc, " edges\r\n");
    temp_command.delta_size = size;
    temp_command.delta = data;

    for (i=0; i<size; i++) {
        uart_print(&bc, _utoa(itoa_buf, data[i]));
        uart_print(&bc, "\r\n");
    }

    ir_send_start(&temp_command);
    // save acquired command to the ir_tome
}

#define PARSER_CNT 16

void parse_user_input(void)
{
#ifdef UART_RX_USES_RINGBUF
    struct ringbuf *rbr = uart_get_rx_ringbuf(&bc);
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
    char *input = uart_get_rx_buf(&bc);
#endif
    char f = input[0];

    if (f == '?') {
        display_menu();
    } else if (f == 's') {
        ir_acquire_start();
        uart_print(&bc, "go\r\n");
    } else if (f == 'r') {
        ir_send_start(&temp_command);
        uart_print(&bc, "sig sent\r\n");
    }
}

