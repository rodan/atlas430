
//  software bitbang of serial protocols
//  currently supported:
//        - i2c master
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         BSD

#include "warning.h"

#ifdef __I2C_CONFIG_H__
#ifdef I2C_USES_BITBANGING

#include <stdint.h>
#include <stdbool.h>
#include <msp430.h>

#include "serial_bitbang.h"

static inline void sbb_sda_high(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_DIR &= ~I2C_MASTER_SDA
    *(uint8_t *) i2c_spec->port_dir &= ~(i2c_spec->pin_sda);
}

static inline void sbb_sda_low(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_DIR |= I2C_MASTER_SDA
    *(uint8_t *) i2c_spec->port_dir |= i2c_spec->pin_sda;
}

static inline void sbb_scl_high(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_DIR &= ~I2C_MASTER_SCL
    *(uint8_t *) i2c_spec->port_dir &= ~(i2c_spec->pin_scl);
}

static inline void sbb_scl_low(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_DIR |= I2C_MASTER_SCL
    *(uint8_t *) i2c_spec->port_dir |= i2c_spec->pin_scl;
}

static inline bool sbb_is_sda_high(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_IN & I2C_MASTER_SDA
    if (*(uint8_t *) i2c_spec->port_in & i2c_spec->pin_sda)
        return true;
    return false;
}

static inline bool sbb_is_scl_high(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_IN & I2C_MASTER_SCL
    if (*(uint8_t *) i2c_spec->port_in & i2c_spec->pin_scl)
        return true;
    return false;
}

inline void sbb_i2cm_init(bus_desc_i2c_sw_master_t *i2c_spec)
{
    //I2C_MASTER_DIR &= ~(I2C_MASTER_SCL | I2C_MASTER_SDA);
    //I2C_MASTER_OUT &= ~(I2C_MASTER_SCL | I2C_MASTER_SDA);
    *(uint8_t *) i2c_spec->port_dir &= ~(i2c_spec->pin_scl | i2c_spec->pin_sda);
    *(uint8_t *) i2c_spec->port_out &= ~(i2c_spec->pin_scl | i2c_spec->pin_sda);
}

// returns one of I2C_OK, I2C_MISSING_SCL_PULLUP and/or I2C_MISSING_SDA_PULLUP
static uint8_t i2cm_start(bus_desc_i2c_sw_master_t *i2c_spec, uint8_t options)
{
    uint8_t rv = 0;

    sbb_i2cm_init(i2c_spec);
    // set both SCL and SDA pins as inputs
    //I2C_MASTER_DIR &= ~(I2C_MASTER_SCL + I2C_MASTER_SDA);
    //I2C_MASTER_OUT &= ~(I2C_MASTER_SDA | I2C_MASTER_SCL);

    // bus is currently inactive

    sbb_sda_high(i2c_spec);
    delay_s;
    sbb_scl_high(i2c_spec);
    delay_s;
    if (!sbb_is_sda_high(i2c_spec)) {
        // pin is low, but should have been pulled high by an external resistor
        rv |= I2C_MISSING_SDA_PULLUP;
    }
    if (!sbb_is_scl_high(i2c_spec)) {
        // pin is low, but should have been pulled high by an external resistor
        rv |= I2C_MISSING_SCL_PULLUP;
    }
    if (rv) {
        return rv;
    }
    if (options & I2C_SHT_INIT) {
        sbb_scl_low(i2c_spec);
        delay_c;
        delay_c;
        sbb_scl_high(i2c_spec);
        sbb_sda_low(i2c_spec);
        sbb_scl_low(i2c_spec);
        sbb_scl_high(i2c_spec);
        sbb_sda_high(i2c_spec);
        sbb_scl_low(i2c_spec);
    } else {
        // i2c start sequence
        sbb_sda_low(i2c_spec);
        delay_s;
        sbb_scl_low(i2c_spec);
        delay_s;
    }
    return I2C_OK;
}

static void i2cm_stop(bus_desc_i2c_sw_master_t *i2c_spec, uint8_t options)
{
    if (options & I2C_SHT_INIT) {
        sbb_sda_high(i2c_spec);
        sbb_scl_high(i2c_spec);
    } else {
        sbb_sda_low(i2c_spec);
        delay_s;
        sbb_scl_high(i2c_spec);
        delay_s;
        sbb_sda_high(i2c_spec);
        delay_s;
    }
}

// returns  I2C_ACK or I2C_NAK
static uint8_t i2cm_tx(bus_desc_i2c_sw_master_t *i2c_spec, const uint8_t data, const uint8_t options)
{
    uint8_t rv;
    register uint8_t i = 0;
    register uint8_t slarw = 0;

    if (options & I2C_NO_ADDR_SHIFT) {
        slarw = data;
    } else if (options & I2C_WRITE) {
        slarw = data << 1;
    } else if (options & I2C_READ) {
        slarw = (data << 1) | BIT0;
    }

    for (i = 0; i < 8; i++) {
        if (slarw & 0x80) {
            sbb_sda_high(i2c_spec);
        } else {
            sbb_sda_low(i2c_spec);
        }
        slarw <<= 1;
        sbb_scl_high(i2c_spec);
        delay_c;
        delay_c;
        while (!sbb_is_scl_high(i2c_spec)) {
            delay_c;         // wait if slave holds the clk low
        }
        sbb_scl_low(i2c_spec);
    }
    sbb_sda_high(i2c_spec);
    delay_c;
    delay_c;
    sbb_scl_high(i2c_spec);
    delay_c;
    delay_c;
    while (!sbb_is_scl_high(i2c_spec)) {
        delay_c;         // wait if slave holds the clk low
    }

    if (sbb_is_sda_high(i2c_spec)) {
        rv = I2C_NAK;
    } else {
        rv = I2C_ACK;
    }
    sbb_scl_low(i2c_spec);
    return rv;
}

// returns one of I2C_ACK, I2C_NAK, I2C_MISSING_SCL_PULLUP or I2C_MISSING_SDA_PULLUP
static uint8_t i2cm_tx_buff(bus_desc_i2c_sw_master_t *i2c_spec, const uint8_t * data, uint16_t data_len, const uint8_t options)
{
    uint8_t rv = I2C_ACK;
    uint16_t i;

    for (i=0;i<data_len;i++) {
        rv = i2cm_tx(i2c_spec, data[i], I2C_NO_ADDR_SHIFT | options);
        if (rv != I2C_ACK) {
            break;
        }
    }

    return rv;
}

static uint8_t i2cm_rx(bus_desc_i2c_sw_master_t *i2c_spec, uint8_t * buf, const uint16_t length, const uint8_t options)
{
    uint8_t data;
    volatile unsigned int i, j;

    if (options & I2C_SDA_WAIT) {
        delay_c;
        // wait until the data line is pulled low
        // this method is used by sensirion sensors
        while (sbb_is_sda_high(i2c_spec)) {
            delay_c;
        }
    }

    for (j = 0; j < length; j++) {
        sbb_sda_high(i2c_spec);
        data = 0;
        i = 0;
        for (; i < 8; ++i) {
            sbb_scl_high(i2c_spec);
            while (!sbb_is_scl_high(i2c_spec)) {
                delay_c;         // wait if slave holds the clk low
            }
            data <<= 1;
            if (sbb_is_sda_high(i2c_spec)) {
                data |= 0x01;
            }
            sbb_scl_low(i2c_spec);
        }
        *buf++ = data;
        // send ack

        if (j != length - 1) {
            sbb_sda_low(i2c_spec);
        }

        if ((j == length - 1) && (options & I2C_LAST_NAK)) {
            // send nack
            sbb_sda_high(i2c_spec);
            delay_c;
            sbb_scl_high(i2c_spec);
            delay_c;
            sbb_scl_low(i2c_spec);
        } else {
            // send ack
            sbb_scl_high(i2c_spec);
            delay_c;
            sbb_scl_low(i2c_spec);
        }
    }
    return I2C_ACK; // FIXME ?
}


uint8_t sbb_i2cm_transfer(bus_desc_i2c_sw_master_t *i2c_spec, const i2c_package_t * pkg)
{
    uint8_t rv;

    // START
    rv = i2cm_start(i2c_spec, pkg->options);
    if (rv != I2C_OK) {
        i2cm_stop(i2c_spec, pkg->options);
        return rv;
    }

    if (pkg->options & I2C_READ) {
        // some devices need to write a register address/command before a read
        if (pkg->addr_len) {
            // SLAVE ADDR + W
            rv = i2cm_tx(i2c_spec, pkg->slave_addr, I2C_WRITE | pkg->options);
            if (rv != I2C_ACK) {
                i2cm_stop(i2c_spec, pkg->options);
                return rv;
            }
            // REGISTER ADDR/COMMAND
            rv = i2cm_tx_buff(i2c_spec, pkg->addr, pkg->addr_len, pkg->options);
            if (rv != I2C_ACK) {
                i2cm_stop(i2c_spec, pkg->options);
                return rv;
            }
            // START REPEAT
            if (pkg->options & I2C_REPEAT_SA_ON_READ) {
                rv = i2cm_start(i2c_spec, pkg->options);
                if (rv == I2C_OK) {
                    rv = i2cm_tx(i2c_spec, pkg->slave_addr, pkg->options);
                }
                if (rv != I2C_ACK) {
                    i2cm_stop(i2c_spec, pkg->options);
                    return rv;
                }
            }
        } else {
            // SLAVE ADDR + R
            rv = i2cm_tx(i2c_spec, pkg->slave_addr, pkg->options);
            if (rv != I2C_ACK) {
                i2cm_stop(i2c_spec, pkg->options);
                return rv;
            }
        }
        rv = i2cm_rx(i2c_spec, pkg->data, pkg->data_len, pkg->options);
    } else if (pkg->options & I2C_WRITE) {
        // SLAVE ADDR
        rv = i2cm_tx(i2c_spec, pkg->slave_addr, pkg->options);
        if (rv != I2C_ACK) {
            i2cm_stop(i2c_spec, pkg->options);
            return rv;
        }

        if (pkg->addr_len) {
            rv = i2cm_tx_buff(i2c_spec, pkg->addr, pkg->addr_len, pkg->options);
            if (rv != I2C_ACK) {
                i2cm_stop(i2c_spec, pkg->options);
                return rv;
            }
        }
        if (pkg->data_len) {
            rv = i2cm_tx_buff(i2c_spec, pkg->data, pkg->data_len, pkg->options);
            if (rv != I2C_ACK) {
                i2cm_stop(i2c_spec, pkg->options);
                return rv;
            }
        }
    }
    
    i2cm_stop(i2c_spec, pkg->options);
    return rv;
}
#endif
#endif
