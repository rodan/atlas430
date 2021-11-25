#ifndef _CONFIG_H_
#define _CONFIG_H_

//#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define USE_UART3
#define UART3_BAUD 57600
#define UART3_RX_USES_RINGBUF
#define UART3_RXBUF_SZ 16
#define UART3_TX_USES_IRQ
#define UART3_TXBUF_SZ 32

//#define LED_SYSTEM_STATES

//#define USE_ITOA_LUT
//#define CONFIG_DEBUG
//#define USE_SIG

#endif
