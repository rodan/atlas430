#ifndef __BUS_H_
#define __BUS_H_

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
#define                  BUS_OK  0x0
#define        BUS_INIT_ERR_ARG  0x1

typedef struct bus_pkt {
    uint16_t size;
    uint8_t *buff;
} bus_pkt_t;

typedef struct bus_desc_i2c_hw_master {
    uint16_t usci_base_addr;
    uint8_t slave_addr;
} bus_desc_i2c_hw_master_t;

typedef struct device {
    uint16_t bus_type;
    void *bus_desc; // pointer to either spi_descriptor, i2c_descriptor, etc
    bus_pkt_t pkt;
} device_t;

#ifdef __cplusplus
extern "C" {
#endif

uint16_t bus_init_i2c_hw_master(device_t *dev, const uint16_t usci_base_addr, const uint8_t slave_addr, bus_desc_i2c_hw_master_t *i2c_desc, uint8_t *buff, const uint16_t buff_size);
uint16_t bus_read(device_t *dev, const uint16_t size, const uint16_t cmd_len, uint8_t *cmd);

#ifdef __cplusplus
}
#endif

#endif
