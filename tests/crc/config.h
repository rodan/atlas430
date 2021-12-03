#ifndef _CONFIG_H_
#define _CONFIG_H_

#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
//#define SMCLK_FREQ_8M
#define SMCLK_FREQ_16M

#define USE_UART0
#define UART0_BAUD 57600
#define UART0_RXBUF_SZ 32
#define UART0_RX_USES_RINGBUF
#define UART0_TXBUF_SZ 16
#define UART0_TX_USES_IRQ

#define ZMODEM_O_BYTESIZE_CRC16
#define ZMODEM_O_BYTESIZE_CRC32

#define HW_CRC16
#define HW_CRC32

#define USE_SIG
//#define CONFIG_DEBUG

#endif
