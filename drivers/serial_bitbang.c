
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
#include "bus.h"
#include "serial_bitbang.h"

extern volatile i2c_transaction_t transfer;

/** set SDA port pin to input direction. the external pullup resistor will bring the pin high.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
*/
static inline void sbb_sda_high(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_DIR &= ~I2C_MASTER_SDA
    *(uint8_t *) i2c_spec->port_dir &= ~(i2c_spec->pin_sda);
}

/** set SDA port pin to output direction, thus setting the pin low.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
*/
static inline void sbb_sda_low(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_DIR |= I2C_MASTER_SDA
    *(uint8_t *) i2c_spec->port_dir |= i2c_spec->pin_sda;
}

/** set SCL port pin to input direction. the external pullup resistor will bring the pin high.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
*/
static inline void sbb_scl_high(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_DIR &= ~I2C_MASTER_SCL
    *(uint8_t *) i2c_spec->port_dir &= ~(i2c_spec->pin_scl);
}

/** set SCL port pin to output direction, thus setting the pin low.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
*/
static inline void sbb_scl_low(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_DIR |= I2C_MASTER_SCL
    *(uint8_t *) i2c_spec->port_dir |= i2c_spec->pin_scl;
}

/** test the logic level of the SDA port pin.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
    @return true if SDA is high
*/
static inline bool sbb_is_sda_high(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_IN & I2C_MASTER_SDA
    if (*(uint8_t *) i2c_spec->port_in & i2c_spec->pin_sda)
        return true;
    return false;
}

/** test the logic level of the SCL port pin.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
    @return true if SCL is high
*/
static inline bool sbb_is_scl_high(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // I2C_MASTER_IN & I2C_MASTER_SCL
    if (*(uint8_t *) i2c_spec->port_in & i2c_spec->pin_scl)
        return true;
    return false;
}

/** place the i2c bitbanged bus into a default inactive state by bringing both signals high. external pullup resistors are required.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
*/
inline void sbb_i2cm_init(bus_desc_i2c_sw_master_t *i2c_spec)
{
    // set both SCL and SDA pins as inputs
    //I2C_MASTER_DIR &= ~(I2C_MASTER_SCL | I2C_MASTER_SDA);
    //I2C_MASTER_OUT &= ~(I2C_MASTER_SCL | I2C_MASTER_SDA);
    *(uint8_t *) i2c_spec->port_dir &= ~(i2c_spec->pin_scl | i2c_spec->pin_sda);
    *(uint8_t *) i2c_spec->port_out &= ~(i2c_spec->pin_scl | i2c_spec->pin_sda);

    transfer.status = I2C_IDLE;
}

/** send a 'Start' or a 'Start repeat' sequence to the i2c bitbanged bus. external pullup resistors are required.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
    @return either I2C_OK or I2C_MISSING
*/
static uint8_t sbb_i2cm_start(bus_desc_i2c_sw_master_t *i2c_spec, uint8_t options)
{
    sbb_i2cm_init(i2c_spec);
    // bus is currently inactive

    sbb_sda_high(i2c_spec);
    delay_s;
    sbb_scl_high(i2c_spec);
    delay_s;
    if (!sbb_is_sda_high(i2c_spec)) {
        // pin is low, but should have been pulled high by an external resistor
        return I2C_PULLUP_MISSING;
    }
    if (!sbb_is_scl_high(i2c_spec)) {
        // pin is low, but should have been pulled high by an external resistor
        return I2C_PULLUP_MISSING;
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

/** send a 'Stop' sequence to the i2c bitbanged bus. external pullup resistors are required.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
*/
static void sbb_i2cm_stop(bus_desc_i2c_sw_master_t *i2c_spec, uint8_t options)
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

/** send a byte to the i2c bitbanged bus. external pullup resistors are required.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
    @param data  byte to be sent
    @param options  see serial_bitbang.h for details
    @return either I2C_ACK or I2C_NAK
*/
static uint8_t sbb_i2cm_tx(bus_desc_i2c_sw_master_t *i2c_spec, const uint8_t data, const uint8_t options)
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
            // wait if slave holds the clk low
            delay_c;
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
        // wait if slave holds the clk low
        delay_c;
    }

    if (sbb_is_sda_high(i2c_spec)) {
        rv = I2C_NAK;
    } else {
        rv = I2C_ACK;
    }
    sbb_scl_low(i2c_spec);
    return rv;
}

/** send an array of bytes to the i2c bitbanged bus. external pullup resistors are required.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
    @param data  buffer containing the bytes to be sent
    @param data_len  size of the buffer
    @param options  see serial_bitbang.h for details
    @return either I2C_ACK or I2C_NAK
*/
static uint8_t sbb_i2cm_tx_buff(bus_desc_i2c_sw_master_t *i2c_spec, const uint8_t * data, uint16_t data_len, const uint8_t options)
{
    uint8_t rv = I2C_ACK;
    uint16_t i;

    for (i=0;i<data_len;i++) {
        rv = sbb_i2cm_tx(i2c_spec, data[i], I2C_NO_ADDR_SHIFT | options);
        if (rv != I2C_ACK) {
            break;
        }
    }

    return rv;
}

/** receive an array of bytes from the i2c bitbanged bus. external pullup resistors are required. can block if slave holds CLK low or SDA high.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
    @param buf  buffer where the data will be placed into
    @param length  how many bytes to read
    @param options  see serial_bitbang.h for details
    @return either I2C_ACK
*/
static uint8_t sbb_i2cm_rx(bus_desc_i2c_sw_master_t *i2c_spec, uint8_t * buf, const uint16_t length, const uint8_t options)
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
                delay_c; // wait if slave holds the clk low
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
    return I2C_ACK; // FIXME - maybe implement timeouts
}

/** begin an i2c transaction defined in pkg. external pullup resistors are required. can block if slave holds CLK low or SDA high.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
    @param pkg  pointer to a package struct that describes the transfer operation
    @return either I2C_ACK, I2C_NAK or I2C_PULLUP_MISSING
*/
uint16_t sbb_i2cm_transfer(bus_desc_i2c_sw_master_t *i2c_spec, const i2c_package_t * pkg)
{
    uint16_t rv;

    if (transfer.status == I2C_BUSY)
        return I2C_BUSY;

    transfer.status = I2C_BUSY;

    // START
    rv = sbb_i2cm_start(i2c_spec, pkg->options);
    if (rv != I2C_OK) {
        transfer.status = I2C_FAILED;
        goto cleanup;
    }

    if (pkg->options & I2C_READ) {
        // some devices need to write a register address/command before a read
        if (pkg->addr_len) {
            // SLAVE ADDR + W
            rv = sbb_i2cm_tx(i2c_spec, pkg->slave_addr, I2C_WRITE | pkg->options);
            if (rv != I2C_ACK) {
                transfer.status = I2C_FAILED;
                goto cleanup;
            }
            // REGISTER ADDR/COMMAND
            rv = sbb_i2cm_tx_buff(i2c_spec, pkg->addr, pkg->addr_len, pkg->options);
            if (rv != I2C_ACK) {
                transfer.status = I2C_FAILED;
                goto cleanup;
            }
            // START REPEAT
            if (pkg->options & I2C_REPEAT_SA_ON_READ) {
                rv = sbb_i2cm_start(i2c_spec, pkg->options);
                if (rv == I2C_OK) {
                    rv = sbb_i2cm_tx(i2c_spec, pkg->slave_addr, pkg->options);
                }
                if (rv != I2C_ACK) {
                    transfer.status = I2C_FAILED;
                    goto cleanup;
                }
            }
        } else {
            // SLAVE ADDR + R
            rv = sbb_i2cm_tx(i2c_spec, pkg->slave_addr, pkg->options);
            if (rv != I2C_ACK) {
                transfer.status = I2C_FAILED;
                goto cleanup;
            }
        }
        rv = sbb_i2cm_rx(i2c_spec, pkg->data, pkg->data_len, pkg->options);
    } else if (pkg->options & I2C_WRITE) {
        // SLAVE ADDR
        rv = sbb_i2cm_tx(i2c_spec, pkg->slave_addr, pkg->options);
        if (rv != I2C_ACK) {
            transfer.status = I2C_FAILED;
            goto cleanup;
        }

        if (pkg->addr_len) {
            rv = sbb_i2cm_tx_buff(i2c_spec, pkg->addr, pkg->addr_len, pkg->options);
            if (rv != I2C_ACK) {
                transfer.status = I2C_FAILED;
                goto cleanup;
            }
        }
        if (pkg->data_len) {
            rv = sbb_i2cm_tx_buff(i2c_spec, pkg->data, pkg->data_len, pkg->options);
            if (rv != I2C_ACK) {
                transfer.status = I2C_FAILED;
                goto cleanup;
            }
        }
    }
    
    transfer.status = I2C_IDLE;
cleanup:
    sbb_i2cm_stop(i2c_spec, pkg->options);
    return rv;
}
#endif
#endif
