
//  code for Honeywell TruStability HSC and SSC pressure sensors
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         BSD
//
//  Honeywell High Accuracy Ceramic (HSC) and Standard Accuracy Ceramic
//  (SSC) Series are piezoresistive silicon pressure sensors.

#ifdef CONFIG_HSC_SSC

#ifdef __I2C_CONFIG_H__

#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include "hsc_ssc.h"
#include "i2c.h"

/* you must define the slave address. you can find it based on the part number:

    _SC_________XA_
    where X can be one of:

    S  - spi (this is not the right library for spi opperation)
    2  - i2c slave address 0x28
    3  - i2c slave address 0x38
    4  - i2c slave address 0x48
    5  - i2c slave address 0x58
    6  - i2c slave address 0x68
    7  - i2c slave address 0x78
*/

// returns  0 if all is fine
//          1 if chip is in command mode
//          2 if old data is being read
//          3 if a diagnostic fault is triggered in the chip
//          I2C err levels if sensor is not properly hooked up

uint8_t HSC_SSC_read(const uint16_t usci_base_addr, const uint8_t slave_addr, struct HSC_SSC_pkt *raw)
{
    uint8_t val[4] = { 0, 0, 0, 0 };
    uint8_t rv = 0;

    i2c_package_t pkg;

    pkg.slave_addr = slave_addr;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = val;
    pkg.data_len = 4;
    pkg.options = I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ;

    i2c_transfer_start(usci_base_addr, &pkg, NULL);

    raw->status = (val[0] & 0xc0) >> 6; // first 2 bits from first byte
    raw->bridge_data = ((val[0] & 0x3f) << 8) + val[1];
    raw->temperature_data = ((val[2] << 8) + (val[3] & 0xe0)) >> 5;
    return rv;
}

uint8_t HSC_SSC_convert(const struct HSC_SSC_pkt raw, uint32_t * pressure,
                   int16_t * temperature, const uint16_t output_min,
                   const uint16_t output_max, const float pressure_min,
                   const float pressure_max)
{
    float t, p;
    p = (float) ((float)raw.bridge_data - (float)output_min) * ((float)pressure_max - (float) pressure_min) / ( (float)output_max - (float) output_min) + (float) pressure_min;
    t = ((float) raw.temperature_data * 0.0977) - 50.0;

    *pressure = p;
    *temperature = 100 * t;
    return 0;
}
#endif // __I2C_CONFIG_H__
#endif // CONFIG_HSC_SSC
