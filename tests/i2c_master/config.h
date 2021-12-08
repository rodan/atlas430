#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "i2c_config.h"

#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define USE_UART0
#define UART0_BAUD 57600
#define UART0_RXBUF_SZ 16
#define UART0_RX_USES_RINGBUF
#define UART0_TXBUF_SZ 32
#define UART0_TX_USES_IRQ

#define USE_I2C_MASTER
#define CONFIG_DS3231
#define CONFIG_CYPRESS_FM24
#define CONFIG_FM24CL64B
//#define CONFIG_FM24V10
#define CONFIG_HSC_SSC
#define CONFIG_TCA6408

//#define USE_WATCHDOG

#define USE_SIG
//#define CONFIG_DEBUG

#endif
