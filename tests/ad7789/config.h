#ifndef _CONFIG_H_
#define _CONFIG_H_

#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define UART_USES_UCA0
#define UART0_BAUD 57600
#define UART0_RXBUF_SZ 32
#define UART0_RX_USES_RINGBUF
#define UART0_TXBUF_SZ 16
//#define UART0_TX_USES_IRQ

#define CONFIG_AD7789
#define SPI_BASE_ADDR EUSCI_B1_BASE

/// Select which USCI module to use
#define SPI_USE_DEV      6    ///< \hideinitializer
/**<    0 = USCIA0 \n
*         1 = USCIA1 \n
*         2 = USCIA2 \n
*         3 = USCIA3 \n
*         4 = USCIB0 \n
*         5 = USCIB1 \n
*         6 = USCIB2 \n
*         7 = USCIB3 \n
*         8 = EUSCIA0 \n
*         9 = EUSCIA1 \n
*         10 = EUSCIA2 \n
*         11 = EUSCIA3 \n
*         12 = EUSCIB0 \n
*         13 = EUSCIB1 \n
*         14 = EUSCIB2 \n
*         15 = EUSCIB3 \n
**/


#define USE_SIG
//#define CONFIG_DEBUG

#endif
