
/*
*/

#include "warning.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "i2c.h"

#ifdef I2C_USES_BITBANGING
#include "serial_bitbang.h"
#endif

#include "bus.h"

uint16_t bus_init_i2c_hw_master(device_t *dev, const uint16_t usci_base_addr, const uint8_t slave_addr, bus_desc_i2c_hw_master_t *i2c_desc)
{
    if (!dev || !usci_base_addr || !slave_addr || !i2c_desc) {
        dev->bus_type = 0;
        return BUS_ERR_ARG;
    }

    i2c_desc->usci_base_addr = usci_base_addr;
    dev->slave_addr = slave_addr;
    dev->state = BUS_STATE_INITIALIZED;
    dev->bus_desc = i2c_desc;
    dev->bus_type = BUS_TYPE_I2C_HW_MASTER;

    return BUS_OK;
}

#ifdef I2C_USES_BITBANGING
uint16_t bus_init_i2c_sw_master(device_t *dev, const uint8_t slave_addr, bus_desc_i2c_sw_master_t *i2c_desc)
{
    if (!dev || !slave_addr || !i2c_desc) {
        dev->bus_type = 0;
        return BUS_ERR_ARG;
    }

    dev->slave_addr = slave_addr;
    sbb_i2cm_init(i2c_desc);
    dev->state = BUS_STATE_INITIALIZED;
    dev->bus_desc = i2c_desc;
    dev->bus_type = BUS_TYPE_I2C_SW_MASTER;

    return BUS_OK;
}
#endif

uint16_t bus_transfer(device_t *dev, uint8_t *buf, const uint16_t buf_sz, const uint8_t *cmd, const uint16_t cmd_sz, const uint8_t options)
{
    i2c_package_t i2c_pkg;
    bus_desc_i2c_hw_master_t *desc_i2c_hw_master_t;
#ifdef I2C_USES_BITBANGING
    uint16_t rv;
    bus_desc_i2c_sw_master_t *desc_i2c_sw_master_t;
#endif

    if ( !dev | !buf | !buf_sz )
        return BUS_STATE_COMM_ERR;

    memset(&i2c_pkg, 0, sizeof(i2c_package_t));
    if (!(options & I2C_WRITE))
        memset(buf, 0, buf_sz);

    switch(dev->bus_type) {
        case BUS_TYPE_I2C_HW_MASTER:

            desc_i2c_hw_master_t = (bus_desc_i2c_hw_master_t *) dev->bus_desc;
            if (options & I2C_USE_CUSTOM_ADDR) {
                i2c_pkg.slave_addr = dev->custom_slave_addr;
            } else {
                i2c_pkg.slave_addr = dev->slave_addr;
            }
            i2c_pkg.addr = (uint8_t *) cmd;
            i2c_pkg.addr_len = cmd_sz;
            i2c_pkg.data = buf;
            i2c_pkg.data_len = buf_sz;
            i2c_pkg.options = options;

            i2c_transfer_start(desc_i2c_hw_master_t->usci_base_addr, &i2c_pkg, NULL);
            break;
#ifdef I2C_USES_BITBANGING
        case BUS_TYPE_I2C_SW_MASTER:

            desc_i2c_sw_master_t = (bus_desc_i2c_sw_master_t *) dev->bus_desc;
            if (options & I2C_USE_CUSTOM_ADDR) {
                i2c_pkg.slave_addr = dev->custom_slave_addr;
            } else {
                i2c_pkg.slave_addr = dev->slave_addr;
            }
            i2c_pkg.addr = (uint8_t *) cmd;
            i2c_pkg.addr_len = cmd_sz;
            i2c_pkg.data = buf;
            i2c_pkg.data_len = buf_sz;
            i2c_pkg.options = options; // I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ;

            rv = sbb_i2cm_transfer(desc_i2c_sw_master_t, &i2c_pkg);
            if (rv != I2C_ACK) {
                return rv;
            }
            break;
#endif
        default:
            return BUS_STATE_COMM_ERR; 
            break;
    }

    return BUS_OK;
}
