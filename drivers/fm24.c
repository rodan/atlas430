
// driver for CYPRESS FM24Vxx f-ram chips
//
//   author:          Petre Rodan <petre.rodan@simplex.ro>
//   available from:  https://github.com/rodan/
//   license:         GNU GPLv3

#include <inttypes.h>

#include "proj.h"
#include "fm24.h"
#include "serial_bitbang.h"

uint8_t fm24_seek(const uint32_t addr)
{
    uint8_t rv = 255;
    uint8_t retry;
    uint32_t c_addr;
    uint8_t i2c_buff[2];
    i2c_package_t pkg;

    // in case a seek beyond the end of device is requested
    // we roll to the beginning since this memory is circular in nature
    if (addr > FM_LA) {
        c_addr = addr % FM_LA - 1;
    } else {
        c_addr = addr;
    }

    i2c_buff[0] = (c_addr & 0xff00) >> 8;
    i2c_buff[1] = c_addr & 0xff;

    pkg.slave_addr = FM24_BA | (c_addr >> 16);
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = i2c_buff;
    pkg.data_len = 2;
    pkg.options = I2C_WRITE;

    for (retry = 0; retry < FM24_MAX_RETRY; retry++) {
#ifdef HARDWARE_I2C
        i2c_transfer_start(&pkg, NULL);
#else
        rv = i2cm_transfer(&pkg);
        if (rv == I2C_ACK) {
            break;
        }
#endif
    }

    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }

#ifdef FM24_HAS_SLEEP_MODE
    fm24_status |= FM24_AWAKE;
#endif
    return EXIT_SUCCESS;
}

uint32_t fm24_read(uint8_t * data, const uint32_t data_len)
{
    uint8_t rv = 0;

    i2c_package_t pkg;
    pkg.slave_addr = FM24_BA;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = data;
    pkg.data_len = data_len;
    pkg.options = I2C_READ | I2C_LAST_NAK;

#ifdef HARDWARE_I2C
    i2c_transfer_start(&pkg, NULL);
#else
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#endif

#ifdef FM24_HAS_SLEEP_MODE
    fm24_status |= FM24_AWAKE;
#endif
    return EXIT_SUCCESS;
}

uint32_t fm24_read_from(uint8_t * data, const uint32_t addr,
                        const uint32_t data_len)
{
    uint32_t rv = EXIT_FAILURE;

    if (fm24_seek(addr) == EXIT_SUCCESS) {
        rv = fm24_read(data, data_len);
    }

    return rv;
}

uint32_t fm24_write(uint8_t * data, const uint32_t addr,
                    const uint32_t data_len)
{
    uint8_t rv = 0;
    uint8_t retry;
    uint32_t c_addr;
    uint8_t i2c_buff[2];

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

    pkg.slave_addr = FM24_BA | (c_addr >> 16);
    pkg.addr = i2c_buff;
    pkg.addr_len = 2;
    pkg.data = data;
    pkg.data_len = data_len;
    pkg.options = I2C_WRITE;

    for (retry = 0; retry < FM24_MAX_RETRY; retry++) {
#ifdef HARDWARE_I2C
        i2c_transfer_start(&pkg, NULL);
#else
        rv = i2cm_transfer(&pkg);
        if (rv == I2C_ACK) {
            m.e += data_len;
            if (m.e > FM_LA) {
                m.e = m.e % FM_LA - 1;
            }
            break;
        }
#endif
    }

#ifdef FM24_HAS_SLEEP_MODE
    fm24_status |= FM24_AWAKE;
#endif
    return 0;
}

#ifdef FM24_HAS_SLEEP_MODE
uint8_t fm24_sleep(void)
{
    uint8_t rv = 0;
    uint8_t i2c_buff[1] = { FM24_BA << 1 };
    uint8_t i2c_data[1] = { FM24_SLEEP };

    i2c_package_t pkg;
    pkg.slave_addr = FM24_RSVD;
    pkg.addr = i2c_buff;
    pkg.addr_len = 1;
    pkg.data = i2c_data;
    pkg.data_len = 1;
    pkg.options = I2C_NO_ADDR_SHIFT | I2C_WRITE;

#ifdef HARDWARE_I2C
    i2c_transfer_start(&pkg, NULL);
#else
    rv = i2cm_transfer(&pkg);
    //if (rv != I2C_ACK) {
    //    return EXIT_FAILURE;
    //}
#endif

    fm24_status &= ~FM24_AWAKE;
    return rv;
}
#endif

uint32_t fm24_data_len(const uint32_t first, const uint32_t last)
{
    uint32_t rv = 0;

    if (last > first) {
        rv = last - first;
    } else if (last < first) {
        rv = FM_LA - first + last + 1;
    }

    return rv;
}
