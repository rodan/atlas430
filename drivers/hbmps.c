
//  code for Honeywell TruStability HSC and SSC pressure sensors
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         BSD
//
//  Honeywell High Accuracy Ceramic (HSC) and Standard Accuracy Ceramic
//  (SSC) Series are piezoresistive silicon pressure sensors.

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "bus.h"
#include "hbmps.h"

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

uint8_t hbmps_read(device_t *dev, struct hbmps_pkt *raw)
{
    uint8_t rv = 0;
    uint8_t *val;

    memset(dev->pkt.buff, 0, HBMPS_BUFF_SIZE);
    bus_read(dev, HBMPS_BUFF_SIZE, 0, NULL);

    val = dev->pkt.buff;
    raw->status = (val[0] & 0xc0) >> 6; // first 2 bits from first byte
    raw->bridge_data = ((val[0] & 0x3f) << 8) + val[1];
    raw->temperature_data = ((val[2] << 8) + (val[3] & 0xe0)) >> 5;

    return rv;
}

uint8_t hbmps_convert(const struct hbmps_pkt *raw, int32_t *pressure,
                   int32_t *temperature, hbmps_spec_t *spec)
{
    int64_t t, p;
    p = (int64_t) ((uint64_t)raw->bridge_data - (int64_t)spec->output_min) * ((int64_t)spec->pressure_max - (int64_t) spec->pressure_min) / ( (int64_t) spec->output_max - (int64_t) spec->output_min) + (int64_t) spec->pressure_min;
    t = ((int64_t) raw->temperature_data * 977) - 500000; // t * 10000

    *pressure = p;
    *temperature = t/100; // return t * 100
    return 0;
}

