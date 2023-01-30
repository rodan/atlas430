#ifndef _CONFIG_H_
#define _CONFIG_H_

#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define UART_USES_UCA0
#define UART_RXBUF_SZ 16
#define UART_RX_USES_RINGBUF
#define UART_TXBUF_SZ 32
#define UART_TX_USES_IRQ

#define SCH_USES_T1A
#define SCH_SLOT_CNT 2

//#define USE_SIG
//#define CONFIG_DEBUG

#endif
