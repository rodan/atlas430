
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

#include "bus.h"
#include "hbmps.h"

// returns  0 if all is fine
//          NA if chip is in command mode
//          NA if old data is being read
//          0x300 if a diagnostic fault is triggered in the chip
//          I2C err levels if sensor is not properly hooked up

uint16_t hbmps_read(device_t *dev, struct hbmps_pkt *raw)
{
    uint16_t rv;
    uint8_t buff[HBMPS_BUFF_SIZE];

    rv = bus_transfer(dev, buff, HBMPS_BUFF_SIZE, NULL, 0, I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ);
    if ( rv != BUS_OK) {
        return rv;
    }

    raw->status = (buff[0] & 0xc0) >> 6; // first 2 bits from first byte
    raw->bridge_data = ((buff[0] & 0x3f) << 8) + buff[1];
    raw->temperature_data = ((buff[2] << 8) + (buff[3] & 0xe0)) >> 5;

    if (raw->status)
        return HBMPS_ERR_DIAG_FAULT;

    return HBMPS_OK;
}

uint16_t hbmps_convert(const struct hbmps_pkt *raw, int32_t *pressure,
                   int32_t *temperature, const hbmps_spec_t *spec)
{
    int32_t t;
    int64_t p;

    p = (int64_t) ((uint64_t)raw->bridge_data - (int64_t)spec->output_min) * ((int64_t)spec->pressure_max - (int64_t) spec->pressure_min) / ( (int64_t) spec->output_max - (int64_t) spec->output_min) + (int64_t) spec->pressure_min;
    t = ((int32_t) raw->temperature_data * 977) - 500000; // t *10000

    *pressure = p;        // return p in pascals
    *temperature = t/100; // return t in degrees C *100
    return 0;
}

