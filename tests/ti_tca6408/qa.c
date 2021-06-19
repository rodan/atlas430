
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "version.h"
#include "qa.h"

#define STR_LEN 64

void display_menu(void)
{
    char str_temp[STR_LEN];

    snprintf(str_temp, STR_LEN,
            "TI TCA6408 test suite v%d.%d --- available commands:\r\n\r\n", COMMIT, BUILD);
    uart0_print(str_temp);
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1mi\e[0m             - display i2c registers\r\n");
    uart0_print(" \e[33;1mr\e[0m             - tiny read test\r\n");
    uart0_print(" \e[33;1mw\e[0m             - tiny write test\r\n");
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char str_temp[STR_LEN];
    uint8_t data_r;

    //memset(data_w, 0xff, 8);

    if (f == '?') {
        display_menu();
    } else if (f == 'r') {
        TCA6408_read(EUSCI_BASE_ADDR, TCA6408_SLAVE_ADDR, &data_r, TCA6408_INPUT);
        snprintf(str_temp, STR_LEN, "input   0x%x\r\n", data_r);
        uart0_print(str_temp);

        TCA6408_read(EUSCI_BASE_ADDR, TCA6408_SLAVE_ADDR, &data_r, TCA6408_OUTPUT);
        snprintf(str_temp, STR_LEN, "output  0x%x\r\n", data_r);
        uart0_print(str_temp);

        TCA6408_read(EUSCI_BASE_ADDR, TCA6408_SLAVE_ADDR, &data_r, TCA6408_POL_INV);
        snprintf(str_temp, STR_LEN, "pol inv 0x%x\r\n", data_r);
        uart0_print(str_temp);

        TCA6408_read(EUSCI_BASE_ADDR, TCA6408_SLAVE_ADDR, &data_r, TCA6408_CONF);
        snprintf(str_temp, STR_LEN, "conf    0x%x\r\n", data_r);
        uart0_print(str_temp);
    } else if (f == 'w') {
        data_r = 0xa;
        TCA6408_write(EUSCI_BASE_ADDR, TCA6408_SLAVE_ADDR, &data_r, TCA6408_CONF);
    } else if (f == 'i') {
        snprintf(str_temp, STR_LEN, "P7SEL0 0x%x, P7SEL1 0x%x\r\n", P7SEL0, P7SEL1);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2CTLW0 0x%x, UCB2CTLW1 0x%x\r\n", UCB2CTLW0, UCB2CTLW1);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2BRW 0x%x, UCBxSTATW 0x%x\r\n", UCB2BRW, UCB2STATW);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2TBCNT 0x%x\r\n", UCB2TBCNT);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2RXBUF 0x%x, UCB2TXBUF 0x%x\r\n", UCB2RXBUF, UCB2TXBUF);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2I2COA0 0x%x, UCB2ADDRX 0x%x\r\n", UCB2I2COA0, UCB2ADDRX);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2ADDMASK 0x%x, UCB2I2CSA 0x%x\r\n", UCB2ADDMASK, UCB2I2CSA);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2IE 0x%x, UCB2IFG 0x%x\r\n", UCB2IE, UCB2IFG);
        uart0_print(str_temp);

        snprintf(str_temp, STR_LEN, "UCB2IV 0x%x\r\n", UCB2IV);
        uart0_print(str_temp);
    } else {
        uart0_print("\r\n");
    }
}

