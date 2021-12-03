
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "version.h"
#include "ui.h"
#include "ir_acquire.h"
#include "ir_send.h"

#define STR_LEN 64

struct ir_tome temp_command;

static const char menu_str[]="\
 available commands:\r\n\r\n\
 \e[33;1m?\e[0m  - show menu\r\n\
 \e[33;1ms\e[0m  - start IR acquisition\r\n\
 \e[33;1mr\e[0m  - replay signal\r\n";

void display_menu(void)
{
    display_version();
    uart0_print(menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart0_print("tsop b");
    uart0_print(_utoa(sconv, BUILD));
    uart0_print("\r\n");
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
    uart0_print("got ");
    uart0_print(_utoa(itoa_buf, size));
    uart0_print(" edges\r\n");
    temp_command.delta_size = size;
    temp_command.delta = data;

    for (i=0; i<size; i++) {
        uart0_print(_utoa(itoa_buf, data[i]));
        uart0_print("\r\n");
    }

    ir_send_start(&temp_command);
    // save acquired command to the ir_tome
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

    if (f == '?') {
        display_menu();
    } else if (f == 's') {
        ir_acquire_start();
        uart0_print("go\r\n");
    } else if (f == 'r') {
        ir_send_start(&temp_command);
        uart0_print("sig sent\r\n");
    }
}

