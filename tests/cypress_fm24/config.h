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

//#define BAUD_9600
//#define BAUD_19200
//#define BAUD_38400
#define BAUD_57600
//#define BAUD_115200

#define UART0_RXBUF_SZ 16
//#define UART0_RX_USES_RINGBUF

//#define UART0_TXBUF_SZ 32
//#define UART0_TX_USES_IRQ

#define LED_SYSTEM_STATES

//#define USE_WATCHDOG
#define HARDWARE_I2C
#define IRQ_I2C

#define CONFIG_CYPRESS_FM24
#define CONFIG_FM24CL64B
//#define CONFIG_FM24V10

// only a few of the F-RAM chips have explicit sleep mode option
//#ifdef CONFIG_FM24V10
//    #define FM24_HAS_SLEEP_MODE
//#endif

//#define USE_ITOA_LUT
//#define CONFIG_DEBUG

#endif
