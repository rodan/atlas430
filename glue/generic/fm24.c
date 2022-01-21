
// driver for CYPRESS FM24Vxx f-ram chips
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/
//   license:         BSD

#include "warning.h"

#ifdef CONFIG_CYPRESS_FM24

#ifdef __I2C_CONFIG_H__

#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "fm24.h"

#ifdef I2C_USES_BITBANGING
#include "serial_bitbang.h"
#else
#include "i2c.h"
#endif

static struct mem_mgmt_t m;

#ifdef FM24_HAS_SLEEP_MODE
static uint8_t fm24_status;
#endif

uint32_t FM24_read(const uint16_t usci_base_addr, const uint8_t slave_addr, uint8_t * data, const uint32_t addr,
                   const uint32_t data_len)
{
    uint32_t c_addr;
    uint8_t i2c_buff[2];
    i2c_package_t pkg;
#ifdef I2C_USES_BITBANGING
    uint8_t rv = EXIT_FAILURE;
#endif

    // * first seek to the required address

    // in case a seek beyond the end of device is requested
    // we roll to the beginning since this memory is circular in nature
    if (addr > FM_LA) {
        c_addr = addr % FM_LA - 1;
    } else {
        c_addr = addr;
    }

    i2c_buff[0] = (c_addr & 0xff00) >> 8;
    i2c_buff[1] = c_addr & 0xff;

    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = slave_addr | (c_addr >> 16);
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = i2c_buff;
    pkg.data_len = 2;
    pkg.options = I2C_WRITE;

#ifdef I2C_USES_BITBANGING
    rv = i2cm_transfer(&pkg);

    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    // * and now do the actual read
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = slave_addr | (c_addr >> 16);
    pkg.data = data;
    pkg.data_len = data_len;
    pkg.options = I2C_READ | I2C_LAST_NAK;

#ifdef I2C_USES_BITBANGING
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    return EXIT_SUCCESS;
}

uint32_t FM24_write(const uint16_t usci_base_addr, const uint8_t slave_addr, uint8_t * data, const uint32_t addr,
                    const uint32_t data_len)
{
    i2c_package_t pkg;
    uint32_t c_addr;
    uint8_t i2c_buff[2];
#ifdef I2C_USES_BITBANGING
    uint8_t rv = 0;
#endif

    // in case a seek beyond the end of device is requested
    // we roll to the beginning since this memory is circular in nature
    if (addr > FM_LA) {
        c_addr = addr % FM_LA - 1;
    } else {
        c_addr = addr;
    }

    m.e = addr;

    i2c_buff[0] = (c_addr & 0xff00) >> 8;
    i2c_buff[1] = c_addr & 0xff;
    
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = slave_addr | (c_addr >> 16);
    pkg.addr = i2c_buff;
    pkg.addr_len = 2;
    pkg.data = data;
    pkg.data_len = data_len;
    pkg.options = I2C_WRITE;

#ifdef I2C_USES_BITBANGING
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return 1;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    m.e += data_len;
    if (m.e > FM_LA) {
        m.e = m.e % FM_LA - 1;
    }
#ifdef FM24_HAS_SLEEP_MODE
    fm24_status |= FM24_AWAKE;
#endif
    return 0;
}

#ifdef FM24_HAS_SLEEP_MODE
uint8_t FM24_sleep(const uint16_t usci_base_addr, const uint8_t slave_addr)
{
    uint8_t rv = 0;
    uint8_t i2c_buff[1] = { slave_addr << 1 };
    uint8_t i2c_data[1] = { FM24_SLEEP };

    i2c_package_t pkg;

    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = FM24_RSVD;
    pkg.addr = i2c_buff;
    pkg.addr_len = 1;
    pkg.data = i2c_data;
    pkg.data_len = 1;
    pkg.options = I2C_NO_ADDR_SHIFT | I2C_WRITE;

#ifdef I2C_USES_BITBANGING
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    fm24_status &= ~FM24_AWAKE;
    return rv;
}
#endif

uint32_t FM24_data_len(const uint32_t first, const uint32_t last)
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
