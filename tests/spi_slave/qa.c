
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"

#define STR_LEN 64

void display_menu(void)
{
    uart0_print("\r\n SPI slave module test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1mi\e[0m             - get reg settings\r\n");
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    //char sconv[18];
    char str_temp[STR_LEN];

    if (f == '?') {
        display_menu();
    } else if (f == 'i') {
        snprintf(str_temp, STR_LEN, "UCB1CTLW0 0x%x, UCB1BRW 0x%x\r\n", UCB1CTLW0, UCB1BRW);
        uart0_print(str_temp);
        snprintf(str_temp, STR_LEN, "P1DIR 0x%x, P1IES 0x%x\r\n", P1DIR, P1IES);
        uart0_print(str_temp);
        snprintf(str_temp, STR_LEN, "P1IE 0x%x, P1IN 0x%x\r\n", P1IE, P1IN);
        uart0_print(str_temp);
    } else {
        uart0_print("\r\n");
    }
}

