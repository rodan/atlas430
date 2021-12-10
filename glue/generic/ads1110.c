
//  library for ADS1110 - Burr-Brown 16bit analog to digital converter
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         GNU GPLv3


#ifdef CONFIG_ADS1110
#ifdef __I2C_CONFIG_H__

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "i2c.h"
#include "ads1110.h"

struct ads1110_t eadc;

#if 0
// doublecheck that CCR2 is not used by another driver
static void ads1110_state_machine(const uint32_t msg)
{
    switch (eadc.state) {
        case STATE_CONVERT:
            // conversion should be ready
            ads1110_read(ED0, &eadc);
            ads1110_convert(&eadc);
            eadc.state = STATE_STANDBY;
        break;
        default:
            // trigger new single conversion
            ads1110_config(ED0, BITS_16 | PGA_2 | SC | ST);
            eadc.state = STATE_CONVERT;
            timer_a0_delay_noblk_ccr2(_10ms * 7);
        break;
    }
}
#endif

uint8_t ADS1110_init(const uint16_t usci_base_addr, const uint8_t slave_address, const uint8_t config_reg)
{
    //eh_register(&ads1110_state_machine, SYS_MSG_TIMER0_CCR2);
    ADS1110_config(usci_base_addr, slave_address, config_reg);

    return EXIT_SUCCESS;
}

// returns  0 if all is fine
//          I2C err levels if sensor is not properly hooked up

uint8_t ADS1110_read(const uint16_t usci_base_addr, const uint8_t slave_addr, struct ads1110_t *adc)
{
    uint8_t val[3] = { 0, 0, 0 };
    //uint8_t rdy;

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = slave_addr;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = val;
    pkg.data_len = 3;
    pkg.options = I2C_READ | I2C_LAST_NAK;

    i2c_transfer_start(usci_base_addr, &pkg, NULL);

    adc->conv_raw = ( val[0] << 8 ) + val[1];
    adc->config = val[2];
    //rdy = (val[2] & 0x80) >> 7;
    return EXIT_SUCCESS;
}

void ADS1110_convert(struct ads1110_t *adc)
{
    uint8_t bits;
    uint8_t PGA; // gain setting (0-3)
    int32_t tmp;

    bits = (adc->config & 0xc) >> 2; // DR aka data rate
    if (bits == 0) {
        bits = 12;
    } else {
        bits += 13;
    }
    PGA = adc->config & 0x3;

    tmp = ((int32_t) adc->conv_raw * 20480) >> (bits - 1 + PGA);
    adc->conv = tmp;
}

// set the configuration register
uint8_t ADS1110_config(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint8_t val)
{

    uint8_t data = val;

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = slave_addr;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = &data;
    pkg.data_len = 1;
    pkg.options = I2C_WRITE;

    i2c_transfer_start(usci_base_addr, &pkg, NULL);

    return EXIT_SUCCESS;
}

#endif
#endif
