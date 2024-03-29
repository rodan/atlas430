
// driver for Texas Instruments TCA6408 IO expander chips
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/
//   license:         BSD

#include "warning.h"

#ifdef CONFIG_TCA6408

#ifdef __I2C_CONFIG_H__

#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include "tca6408.h"
#include "i2c.h"

uint8_t TCA6408_read(const uint16_t usci_base_addr, const uint8_t slave_addr, uint8_t * data, const uint8_t addr)
{
    i2c_package_t pkg;
    uint8_t i2c_buf[1];

    i2c_buf[0] = addr;

    // first seek to the required address
    pkg.slave_addr = slave_addr;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = i2c_buf;
    pkg.data_len = 1;
    pkg.options = I2C_WRITE;

    i2c_transfer_start(usci_base_addr, &pkg, NULL);

    // and now do the actual read
    pkg.data = data;
    pkg.data_len = 1;
    pkg.options = I2C_READ | I2C_LAST_NAK;

    i2c_transfer_start(usci_base_addr, &pkg, NULL);

    return EXIT_SUCCESS;
}

uint8_t TCA6408_write(const uint16_t usci_base_addr, const uint8_t slave_addr, uint8_t * data, const uint8_t addr)
{
    i2c_package_t pkg;
    uint8_t i2c_buf[1];

    i2c_buf[0] = addr;

    pkg.slave_addr = slave_addr;
    pkg.addr = i2c_buf;
    pkg.addr_len = 1;
    pkg.data = data;
    pkg.data_len = 1;
    pkg.options = I2C_WRITE;

    i2c_transfer_start(usci_base_addr, &pkg, NULL);

    return EXIT_SUCCESS;
}

#endif // __I2C_CONFIG_H__
#endif // CONFIG_TCA6408
