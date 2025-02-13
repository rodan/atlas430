
/*
*/

#include "warning.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "i2c.h"
#include "bus.h"

uint16_t bus_init_i2c_hw_master(device_t *dev, const uint16_t usci_base_addr, const uint8_t slave_addr, bus_desc_i2c_hw_master_t *i2c_desc, uint8_t *buff, const uint16_t buff_size)
{
    if (!dev || !buff || !buff_size || !usci_base_addr || !slave_addr || !i2c_desc) {
        dev->bus_type = 0;
        return BUS_INIT_ERR_ARG;
    }

    i2c_desc->usci_base_addr = usci_base_addr;
    i2c_desc->slave_addr = slave_addr;
    dev->bus_desc = i2c_desc;
    dev->pkt.buff = buff;
    dev->pkt.size = buff_size;
    dev->bus_type = BUS_TYPE_I2C_HW_MASTER;

    return BUS_OK;
}

uint16_t bus_read(device_t *dev, const uint16_t size, const uint16_t cmd_len, uint8_t *cmd)
{
    i2c_package_t pkg;
    bus_desc_i2c_hw_master_t *desc_i2c_hw_master_t;

    memset(dev->pkt.buff, 0, dev->pkt.size);

    switch(dev->bus_type) {
        case BUS_TYPE_I2C_HW_MASTER:

            memset(&pkg, 0, sizeof(i2c_package_t));
            desc_i2c_hw_master_t = (bus_desc_i2c_hw_master_t *) dev->bus_desc;
            pkg.slave_addr = desc_i2c_hw_master_t->slave_addr;
            pkg.addr = cmd;
            pkg.addr_len = cmd_len;
            pkg.data = dev->pkt.buff;
            pkg.data_len = size;
            pkg.options = I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ;

            i2c_transfer_start(desc_i2c_hw_master_t->usci_base_addr, &pkg, NULL);
            break;
        default:
            return BUS_STATE_COMM_ERR; 
            break;
    }

    return BUS_OK;
}

#if 0
#ifdef __I2C_CONFIG_H__

#ifdef I2C_USES_BITBANGING
#include "serial_bitbang.h"
#else
#include "i2c.h"
#endif
#endif
#endif
