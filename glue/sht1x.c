
//  code for sensirion sht 1x humidity and temperature sensors
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         BSD

#ifdef __I2C_CONFIG_H__

#include <msp430.h>
#include <stdlib.h>
#include <stddef.h>
#include "sht1x.h"
#include "serial_bitbang.h"

uint8_t SHT1X_i2csens_rxfrom(const uint8_t slave_addr, uint8_t * data,
                       uint16_t data_len)
{
    uint8_t rv;

    i2c_package_t pkg;
    pkg.slave_addr = slave_addr;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = data;
    pkg.data_len = data_len;
    pkg.options = I2C_READ | I2C_SHT_INIT | I2C_NO_ADDR_SHIFT | I2C_SDA_WAIT;

    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

uint8_t SHT1X_get_status(uint8_t * data)
{
    return SHT1X_i2csens_rxfrom(0x7, data, 1);
}

uint8_t SHT1X_get_meas(int16_t * temp, uint16_t * rh)
{
    uint8_t raw_temp[3];
    uint8_t raw_rh[3];
    uint16_t raw_temp_l, raw_rh_l;
    float temp_f, rh_f, rhc_f;

    if (SHT1X_i2csens_rxfrom(0x3, raw_temp, 3) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if (SHT1X_i2csens_rxfrom(0x5, raw_rh, 3) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    raw_temp_l = (raw_temp[0] << 8) + raw_temp[1];
    raw_rh_l = (raw_rh[0] << 8) + raw_rh[1];
    temp_f = (D1 + (D2 * (float)(raw_temp_l))) * 100.0;
    rh_f = C1 + (C2 * raw_rh_l) + (C3 * raw_rh_l * raw_rh_l);
    // temperature compensation
    rhc_f = ((temp_f / 100.0 - 25.0) * (T1 + (T2 * raw_rh_l)) + rh_f) * 100.0;
    *temp = (int16_t) temp_f;
    *rh = (uint16_t) rhc_f;

    return EXIT_SUCCESS;
}

void SHT1X_i2csens_reset(void)
{
    uint8_t i;
    sda_high;
    delay_c;
    for (i = 0; i < 30; i++) {
        scl_low;
        scl_high;
    }
}

#endif // __I2C_CONFIG_H__

