#ifndef _CONFIG_H_
#define _CONFIG_H_

#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define UART_USES_UCA1
#define UART1_BAUD 57600
#define UART1_RXBUF_SZ 16
#define UART1_RX_USES_RINGBUF
#define UART1_TXBUF_SZ 32
#define UART1_TX_USES_IRQ

#define SPI_USES_UCB1
#define SPI_BASE_ADDR EUSCI_B1_BASE

#define CONFIG_DS3234

//#define USE_WATCHDOG

#define USE_SIG
//#define CONFIG_DEBUG

#endif
