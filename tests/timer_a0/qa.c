
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"
#include "timer_a0.h"
#include "uart3.h"

#define STR_LEN 64

void display_menu(void)
{
    uart0_print("\r\n uart0 and helper module test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1m1\e[0m             - send first command\r\n" );
    uart0_print(" \e[33;1m2\e[0m             - send command 2\r\n" );
    uart0_print(" \e[33;1m3\e[0m             - send command 3\r\n" );
    uart0_print(" \e[33;1m4\e[0m             - send command 4\r\n" );
}

void parse_user_input(void)
{
    uint8_t i;
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char sync[] = { 0x55 };
    char cmd1[] = { 0x0a, 0x0a, 0x0a, 0x1e, 0x1e, 0x30, 0x30, 0x31, 0x46, 0x30, 0x38, 0x3, 0x7e };
    char cmd2[] = { 0x68, 0xda, 0xb9, 0x1e, 0x1e, 0x39, 0x38, 0x31, 0x46, 0x31, 0x30, 0x3, 0x88 };
    char cmd3[] = { 0x68, 0xda, 0xb9, 0x1f, 0x1f, 0x30, 0x30, 0x30, 0x41, 0x3, 0x12 };

    //char itoa_buf[18];
    //uint32_t in;

    if (f == '?') {
        display_menu();
    } else if (f == '1') {
        uart3_tx_str(sync, 1);
        sig2_off;
        timer_a0_delay_ccr2(500);
        sig2_on;
        uart3_tx_str(cmd1, 13);
        uart0_print("cmd1 sent\r\n");
    } else if (f == '2') {
        uart3_tx_str(sync, 1);
        sig2_off;
        timer_a0_delay_ccr2(500);
        sig2_on;
        uart3_tx_str(cmd2, 13);
        uart0_print("cmd2 sent\r\n");
    } else if (f == '3') {
        uart0_print(" * cmd3 ");
        for (i=0; i<50; i++) {
            uart3_tx_str(sync, 1);
            sig2_off;
            timer_a0_delay_ccr2(500);
            sig2_on;
            uart3_tx_str(cmd3, 11);
            //uart0_print("+");
            timer_a0_delay_ccr2(3800);
        }
        uart0_print(" done\r\n");
    } else {
        uart0_print("\r\n");
    }
}

void parse_sensor_output(void)
{
    //char *input = uart3_get_rx_buf();
    //char itoa_buf[18];

    uart0_print("sensor returned ");
    uart0_print("something");
    //uart0_print(input);
    uart0_print("\r\n");
}

