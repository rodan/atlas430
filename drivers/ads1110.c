
//  library for ADS1110 - Burr-Brown 16bit analog to digital converter
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         GNU GPLv3

#include "warning.h"

#ifdef CONFIG_ADS1110
#ifdef __I2C_CONFIG_H__

#include <stdint.h>
#include <stddef.h>

#include "bus.h"
#include "ads1110.h"

struct ads1110_t eadc;

uint16_t ads1110_config(device_t *dev, const uint8_t val)
{
    uint16_t rv;
    uint8_t buf[1] = { val };

    rv = bus_transfer(dev, buf, 1, NULL, 0, I2C_WRITE);
    return rv;
}


uint16_t ads1110_init(device_t *dev, const uint8_t config_reg)
{
    uint16_t rv;
    eadc.state = ADS1110_STATE_CONVERT;
    rv = ads1110_config(dev, config_reg);

    return rv;
}

uint16_t ads1110_read(device_t *dev, struct ads1110_t *adc)
{
    uint16_t rv;
    uint8_t val[3] = { 0, 0, 0 };

    rv = bus_transfer(dev, val, 3, NULL, 0, I2C_READ | I2C_LAST_NAK );
    if (rv == BUS_OK) {
        adc->conv_raw = ( val[0] << 8 ) + val[1];
        adc->config = val[2];
    } else {
        adc->conv_raw = 0;
        adc->config = 0;
    }

    return rv;
}

void ads1110_convert(struct ads1110_t *adc)
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

#endif
#endif
