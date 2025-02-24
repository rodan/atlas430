#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "i2c_config.h"

#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define UART_USES_UCA0
#define UART_USES_UCA1
#define UART_USES_UCA2
#define UART_USES_UCA3
#define UART_RXBUF_SZ 16
#define UART_RX_USES_RINGBUF
#define UART_TXBUF_SZ 32
#define UART_TX_USES_IRQ

#define USE_I2C_MASTER
#define CONFIG_DS3231
#define CONFIG_CYPRESS_FM24
#define CONFIG_FM24CL64B
//#define CONFIG_FM24V10
#define CONFIG_HSC_SSC
#define CONFIG_TCA6408

//#define USE_WATCHDOG

#define USE_SIG
#define CONFIG_DEBUG

//#define TEST_CYPRESS_FM24
//#define TEST_HBMPS
#define TEST_DSRTC
//#define TEST_TCA6408

#endif
