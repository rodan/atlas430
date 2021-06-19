
#include <stdio.h>
#include <string.h>

#include "uart0.h"
#include "i2c.h"
#include "sht1x.h"
#include "qa.h"

#define STR_LEN 64

void display_menu(void)
{
    char str_temp[STR_LEN];

    snprintf(str_temp, STR_LEN, "\r\n Sensirion SHT1x test suite --- available commands:\r\n\r\n");
    uart0_print(str_temp);
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1mi\e[0m             - display i2c registers\r\n");
    uart0_print(" \e[33;1mr\e[0m             - read sensor\r\n");
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char str_temp[STR_LEN];

    uint8_t reg;
    int16_t temperature;
    uint16_t rh;

    if (f == '?') {
        display_menu();
    } else if (f == 'r') {
        SHT1X_get_status(&reg);
        snprintf(str_temp, STR_LEN, "reg: 0x%x\r\n", reg);
        uart0_print(str_temp);

        SHT1X_get_meas(&temperature, &rh);
        snprintf(str_temp, STR_LEN, "temperature: %d.%d degC\r\n", temperature / 100, temperature % 100);
        uart0_print(str_temp);
        snprintf(str_temp, STR_LEN, "humidity: %d.%d %%rh\r\n", rh / 100, rh % 100);
        uart0_print(str_temp);
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
