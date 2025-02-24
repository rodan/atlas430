#ifndef __BUS_H_
#define __BUS_H_

#include <stdint.h>
#include "i2c.h"

// bus.type
#define  BUS_TYPE_I2C_HW_MASTER  0x1
#define  BUS_TYPE_I2C_SW_MASTER  0x2
#define   BUS_TYPE_I2C_HW_SLAVE  0x3
#define   BUS_TYPE_I2C_SW_SLAVE  0x4
#define  BUS_TYPE_SPI_HW_MASTER  0x5
#define  BUS_TYPE_SPI_SW_MASTER  0x6
#define   BUS_TYPE_SPI_HW_SLAVE  0x7
#define   BUS_TYPE_SPI_SW_SLAVE  0x8

// bus.state
#define   BUS_STATE_INITIALIZED  0x80
#define          BUS_STATE_BUSY  0x40
#define      BUS_STATE_COOLDOWN  0x20
#define      BUS_STATE_COMM_ERR  0x10

// bus_init_*() return errors
#define                  BUS_OK  0x00
#define             BUS_ERR_ARG  0x01

typedef struct bus_desc_i2c_hw_master {
    uint16_t state;
    uint16_t usci_base_addr;
    uint8_t slave_addr;
} bus_desc_i2c_hw_master_t;

typedef struct bus_desc_i2c_sw_master {
    uint16_t state;
    uintptr_t port_dir;
    uintptr_t port_out;
    uintptr_t port_in;
    uint16_t pin_scl;
    uint16_t pin_sda;
    uint8_t slave_addr;
} bus_desc_i2c_sw_master_t;

typedef struct device {
    uint16_t bus_type;
    void *bus_desc; // pointer to either spi_descriptor, i2c_descriptor, etc
    void *priv;
} device_t;

#ifdef __cplusplus
extern "C" {
#endif

/** initialize a device that uses a hardware i2c master bus. no memory is allocated by this function.
    @param dev             device driver pointer
    @param usci_base_addr  microcontroller universal serial communication interface base address
    @param slave_addr      chip i2c slave address
    @param i2c_desc        pre-allocated struct that holds usci_base_addr and slave_addr
    @return BUS_OK on success
*/
uint16_t bus_init_i2c_hw_master(device_t *dev, const uint16_t usci_base_addr, const uint8_t slave_addr, bus_desc_i2c_hw_master_t *i2c_desc);

/** initialize a device that uses a bitbanged i2c master bus. no memory is allocated by this function.
    @param dev             device driver pointer
    @param slave_addr      chip i2c slave address
    @param i2c_desc        pre-allocated struct that holds usci_base_addr and slave_addr
    @return BUS_OK on success
*/
uint16_t bus_init_i2c_sw_master(device_t *dev, const uint8_t slave_addr, bus_desc_i2c_sw_master_t *i2c_desc);

/** issue a read or write i2c transaction on the bus defined in dev->bus_desc. no memory is allocated by this function.
    @param dev     device driver pointer
    @param buf     pre-allocated buffer where the read data is stored
    @param buf_sz  number of bytes read from the bus
    @param cmd     preamble command or address to send to the bus before clocking out buf. can be NULL
    @param cmd_sz  number of bytes in the preamble. can be 0
    @param options see i2c.h for details
    @return BUS_OK on success
*/
uint16_t bus_transfer(device_t *dev, uint8_t *buf, const uint16_t buf_sz, const uint8_t *cmd, const uint16_t cmd_sz, const uint8_t options);

#ifdef __cplusplus
}
#endif

#endif
