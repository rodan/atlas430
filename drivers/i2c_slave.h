#ifndef _I2C_SLAVE_H_
#define _I2C_SLAVE_H_

#include <stdint.h>

uint8_t *i2c_slave_tx_data_start_addr;
uint8_t *i2c_slave_tx_data;

volatile uint8_t *i2c_slave_rx_data_start_addr;
volatile uint8_t *i2c_slave_rx_data;

#define I2C_RX_BUFF_LEN     8
volatile uint8_t i2c_rx_buff[I2C_RX_BUFF_LEN];
volatile uint8_t i2c_rx_rdy;
uint8_t i2c_rx_ctr;

void i2c_slave_init(void);
void i2c_slave_uninit(void);

enum i2c_tevent {
    I2C_EV_RX = BIT0,
    I2C_EV_TX = BIT1
};    

volatile enum i2c_tevent i2c_last_event;

#endif
