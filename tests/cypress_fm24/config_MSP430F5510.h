#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "i2c_config.h"

#define USE_XT1
//#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define USE_UART1
#define UART1_BAUD 57600
#define UART1_RXBUF_SZ 16
#define UART1_RX_USES_RINGBUF
#define UART1_TXBUF_SZ 32
#define UART1_TX_USES_IRQ

#define I2C_USES_IRQ

#define CONFIG_CYPRESS_FM24
//#define CONFIG_FM24CL64B
#define CONFIG_FM24V10

//#define USE_WATCHDOG

#define USE_SIG
//#define CONFIG_DEBUG

#endif
