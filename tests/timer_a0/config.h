#ifndef _CONFIG_H_
#define _CONFIG_H_

#define USE_XT1

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define USE_UART0
#define UART0_BAUD 57600
#define UART0_RXBUF_SZ 16
//#define UART0_RX_USES_RINGBUF
#define UART0_TXBUF_SZ 32
//#define UART0_TX_USES_IRQ

#define USE_UART3
#define UART3_BAUD 57600
#define UART3_RXBUF_SZ 16
//#define UART3_RX_USES_RINGBUF
#define UART3_TXBUF_SZ 32
//#define UART3_TX_USES_IRQ


#if UART3_BAUD == 19200
    #define INTRCHAR_TMOUT  500 // _2ms
#elif UART3_BAUD == 38400
    #define INTRCHAR_TMOUT  250
#elif UART3_BAUD == 57600
    #define INTRCHAR_TMOUT  166
#elif UART3_BAUD == 115200
    #define INTRCHAR_TMOUT  83
#endif

//#define USE_ITOA_LUT
//#define CONFIG_DEBUG

#endif
