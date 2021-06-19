
#include <string.h>
#include <inttypes.h>
#include "config.h"
#include "proj.h"
#include "glue.h"
#include "version.h"
#include "qa.h"

struct HSC_SSC_pkt ps;

#define STR_LEN 64
char str_temp[STR_LEN];

void display_menu(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];
    
    uart0_print("\r\n Honeywell HSC v");
    uart0_print(_utoa(sconv, COMMIT));
    uart0_print(".");
    uart0_print(_utoa(sconv, BUILD));
    uart0_print(" test suite\r\n");
    uart0_print(" --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m  - show menu\r\n");
    uart0_print(" \e[33;1mi\e[0m  - display i2c registers\r\n");
    uart0_print(" \e[33;1mr\e[0m  - read current values\r\n");
}

void parse_user_input(void)
{
    char *input;
    uint8_t p;

    uint32_t pressure;
    int16_t temperature;

    char sconv[CONV_BASE_10_BUF_SZ];


    input = uart0_get_rx_buf();
    p = input[0];

    if (p == '?') {
        display_menu();
    } else if (p == 'r') {       // [r]ead value
        HSC_SSC_read(EUSCI_BASE_ADDR, SLAVE_ADDR, &ps);
        HSC_SSC_convert(ps, &pressure, &temperature, OUTPUT_MIN, OUTPUT_MAX, PRESSURE_MIN,
                   PRESSURE_MAX);

        uart0_print("status: ");
        uart0_print(_utoh(sconv, ps.status));
        uart0_print("\r\nbridge_data: ");
        uart0_print(_utoh(sconv, ps.bridge_data));
        uart0_print("\r\ntemperature_data: ");
        uart0_print(_utoh(sconv, ps.temperature_data));
        uart0_print("\r\npressure: ");
        uart0_print(_utoa(sconv, pressure));
        uart0_print("\r\ntemperature: ");
        if ( temperature<0 ) {
            uart0_print("-");
        }
        uart0_print(_utoa(sconv, abs(temperature / 100)));
        uart0_print(".");
        uart0_print(prepend_padding(sconv, _utoa(sconv, abs(temperature % 100)), PAD_ZEROES, 2));
        uart0_print("\r\n");
    } else if (p == 'i') {   //  [i]2c registers
        uart0_print("P7SEL0 ");
        uart0_print(_utoh(sconv, P7SEL0));
        uart0_print(" P7SEL1 ");
        uart0_print(_utoh(sconv, P7SEL1));
        uart0_print("\r\nUCB2CTLW0 ");
        uart0_print(_utoh(sconv, UCB2CTLW0));
        uart0_print(" UCB2CTLW1 ");
        uart0_print(_utoh(sconv, UCB2CTLW1));
        uart0_print("\r\nUCB2BRW ");
        uart0_print(_utoh(sconv, UCB2BRW));
        uart0_print(" UCB2STATW ");
        uart0_print(_utoh(sconv, UCB2STATW));
        uart0_print("\r\nUCB2TBCNT ");
        uart0_print(_utoh(sconv, UCB2TBCNT));
        uart0_print("\r\nUCB2RXBUF ");
        uart0_print(_utoh(sconv, UCB2RXBUF));
        uart0_print(" UCB2TXBUF ");
        uart0_print(_utoh(sconv, UCB2TXBUF));
        uart0_print("\r\nUCB2I2COA0 ");
        uart0_print(_utoh(sconv, UCB2I2COA0));
        uart0_print(" UCB2ADDRX ");
        uart0_print(_utoh(sconv, UCB2ADDRX));
        uart0_print("\r\nUCB2ADDMASK ");
        uart0_print(_utoh(sconv, UCB2ADDMASK));
        uart0_print(" UCB2I2CSA ");
        uart0_print(_utoh(sconv, UCB2I2CSA));
        uart0_print("\r\nUCB2IE ");
        uart0_print(_utoh(sconv, UCB2IE));
        uart0_print(" UCB2IFG ");
        uart0_print(_utoh(sconv, UCB2IFG));
        uart0_print("\r\nUCB2IV ");
        uart0_print(_utoh(sconv, UCB2IV));
        uart0_print("\r\n");
    } else {
        uart0_print("\r\n");
    }
    uart0_set_eol();
}
