
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"
#include "ir_acquire.h"
#include "ir_send.h"

#define STR_LEN 64

struct ir_tome temp_command;

void display_menu(void)
{
    uart0_print("\r\n vishay TSOP aquisition module test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m  - show menu\r\n");
    uart0_print(" \e[33;1ms\e[0m  - start IR acquisition\r\n" );
    uart0_print(" \e[33;1mr\e[0m  - replay signal\r\n" );
    
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

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char itoa_buf[18];

    if (f == '?') {
        display_menu();
    } else if (f == 'i') {
        uart0_print("P1IN ");
        uart0_print(_utob(itoa_buf, P1IN));
        uart0_print("\r\n P1IE");
        uart0_print(_utob(itoa_buf, P1IE));
        uart0_print("\r\n P1IES");
        uart0_print(_utob(itoa_buf, P1IES));
        uart0_print("\r\n P1IFG");
        uart0_print(_utob(itoa_buf, P1IFG));
        uart0_print("\r\n");
    } else if (f == 's') {
        ir_acquire_start();
        uart0_print("go\r\n");
    } else if (f == 'r') {
        ir_send_start(&temp_command);
        uart0_print("sig sent");
    } else {
        uart0_print("\r\n");
    }
}

