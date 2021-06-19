#ifndef _CONFIG_H_
#define _CONFIG_H_

#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define USE_UART0

//#define BAUD_9600
//#define BAUD_19200
//#define BAUD_38400
#define BAUD_57600
//#define BAUD_115200

#define UART0_RXBUF_SZ 32
#define UART0_RX_USES_RINGBUF

#define UART0_TXBUF_SZ 16
//#define UART0_TX_USES_IRQ

#define CONFIG_AD7789
#define EUSCI_SPI_BASE_ADDR EUSCI_B1_BASE
#define AD7789_CS_HIGH      P5OUT |= BIT3
#define AD7789_CS_LOW       P5OUT &= ~BIT3

//#define USE_ITOA_LUT
//#define CONFIG_DEBUG

#endif
