#ifndef __AD7789_H_
#define __AD7789_H_

// register bits
#define AD7789_REN          0x80 // non write-enable bit
#define AD7789_READ         0x08 // continuous read of the data register
#define AD7789_CREAD        0x04 // continuous read of the data register

// register addresses
#define AD7789_COMM         0x00 // 8 bits
#define AD7789_STATUS       0x00 // 8 bits
#define AD7789_MODE         0x01 // 8 bits
#define AD7789_DATA         0x03 // 24 bits 

// channel selection
#define AD7789_IN_NORMAL    0x00 // AIN+ AIN-
#define AD7789_IN_SHORT     0x02 // AIN- AIN-
#define AD7789_IN_VDD       0x03 // Vdd monitor

void AD7789_init(const uint16_t baseAddress);
void AD7789_postinit(const uint16_t baseAddress);
void AD7789_init_port(void);
uint8_t AD7789_get_status(const uint16_t baseAddress, uint8_t *status);
uint8_t AD7789_get_conv(const uint16_t baseAddress, uint32_t *conv);
uint8_t AD7789_rst(const uint16_t baseAddress);
void AD7789_deinit_spi(void);

#endif
