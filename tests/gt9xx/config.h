#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "i2c_config.h"

#define USE_XT1
#define USE_XT2

//#define SMCLK_FREQ_1M
//#define SMCLK_FREQ_4M
#define SMCLK_FREQ_8M
//#define SMCLK_FREQ_16M

#define USE_UART0
#define UART0_BAUD 57600
#define UART0_RXBUF_SZ 16
//#define UART0_RX_USES_RINGBUF
//#define UART0_TXBUF_SZ 32
//#define UART0_TX_USES_IRQ

//#define CONFIG_DEBUG

//#define USE_WATCHDOG
#define I2C_USES_IRQ

#define  GT9XX_SA 0x5d
//#define  GT9XX_SA 0x14
#define GT9XX_IRQ BIT3 // port for the irq functionality
#define GT9XX_RST BIT2 // port for the reset functionality
#define GT9XX_CONF_VER_F

// read 50bytes of status registers after every interrupt
//#define DEBUG_STATUS

// if enabled automatically calculate the checksum for the written firmware
// otherwise abort writing if checksum is incorrect
#define OVERWRITE_CHECKSUM

// enable in order to have the following led setup:
// led 2 - on during timer interrupt operation
// led 3 - on during meter uart operation
// led 4 - on when uC is outside of LPM3 (low power mode)
//#define LED_SYSTEM_STATES

#endif
