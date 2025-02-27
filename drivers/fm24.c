
// driver for CYPRESS FM24Vxx f-ram chips
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/
//   license:         BSD

#include "warning.h"

#ifdef CONFIG_CYPRESS_FM24

#ifdef __I2C_CONFIG_H__

#include <stdint.h>
#include <stddef.h>
#include "bus.h"
#include "fm24.h"

static struct mem_mgmt_t m;

#ifdef FM24_HAS_SLEEP_MODE
static uint8_t fm24_status;
#endif

uint16_t fm24_read(device_t *dev, const uint32_t offset, uint8_t *buf, const uint32_t nbytes)
{
    uint16_t rv;
    uint32_t c_addr;
    uint8_t pre_data[2];

    // * first seek to the required address
    // in case a seek beyond the end of device is requested
    // we roll to the beginning since this memory is circular in nature
    if (offset > FM_LA) {
        c_addr = offset % FM_LA - 1;
    } else {
        c_addr = offset;
    }
    pre_data[0] = (c_addr & 0xff00) >> 8;
    pre_data[1] = c_addr & 0xff;

    dev->custom_slave_addr = dev->slave_addr | (c_addr >> 16);
    rv = bus_transfer(dev, pre_data, 2, NULL, 0, I2C_WRITE | I2C_USE_CUSTOM_ADDR);
    if (rv != BUS_OK) {
        return rv;
    }

    // * and now do the actual read
    dev->custom_slave_addr = dev->slave_addr | (c_addr >> 16);
    rv = bus_transfer(dev, buf, nbytes, NULL, 0, I2C_READ | I2C_LAST_NAK | I2C_USE_CUSTOM_ADDR);
    if (rv != BUS_OK) {
        return rv;
    }

    return rv;
}

uint16_t fm24_write(device_t *dev, const uint32_t offset, uint8_t *buf, const uint32_t nbytes)
{
    uint16_t rv;
    uint32_t c_addr;
    uint8_t pre_data[2];

    // in case a seek beyond the end of device is requested
    // we roll to the beginning since this memory is circular in nature
    if (offset > FM_LA) {
        c_addr = offset % FM_LA - 1;
    } else {
        c_addr = offset;
    }
    m.e = offset;

    pre_data[0] = (c_addr & 0xff00) >> 8;
    pre_data[1] = c_addr & 0xff;

    dev->custom_slave_addr = dev->slave_addr | (c_addr >> 16);
    rv = bus_transfer(dev, buf, nbytes, pre_data, 2, I2C_WRITE | I2C_USE_CUSTOM_ADDR);
    if (rv != BUS_OK) {
        return rv;
    }

    m.e += nbytes;
    if (m.e > FM_LA) {
        m.e = m.e % FM_LA - 1;
    }
#ifdef FM24_HAS_SLEEP_MODE
    fm24_status |= FM24_AWAKE;
#endif

    return 0;
}

#ifdef FM24_HAS_SLEEP_MODE
uint16_t fm24_sleep(device_t *dev)
{
    uint8_t rv = 0;
    uint8_t pre_data[1] = { slave_addr << 1 };
    uint8_t buf[1] = { FM24_SLEEP };

    dev->custom_slave_addr = FM24_RSVD;
    rv = bus_transfer(dev, buf, 1, pre_data, 1, I2C_NO_ADDR_SHIFT | I2C_WRITE | I2C_USE_CUSTOM_ADDR);
    if (rv != BUS_OK) {
        return rv;
    }

    fm24_status &= ~FM24_AWAKE;
    return rv;
}
#endif

uint16_t fm24_data_len(const uint32_t first, const uint32_t last)
{
    uint32_t rv = 0;

    if (last > first) {
        rv = last - first;
    } else if (last < first) {
        rv = FM_LA - first + last + 1;
    }

    return rv;
}

#endif // __I2C_CONFIG_H__
#endif // CONFIG_CYPRESS_FM24
