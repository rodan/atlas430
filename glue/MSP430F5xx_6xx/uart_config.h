#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

#define        BAUDRATE_9600  0x1
#define       BAUDRATE_19200  0x2
#define       BAUDRATE_38400  0x3
#define       BAUDRATE_57600  0x4
#define      BAUDRATE_115200  0x5


// values based on slau208q.pdf Table 36-4 Commonly Used Baud Rates

// UCAxBR0  - clock prescaler (8bit) LSB
// UCAxBR1  - clock prescaler (8bit) MSB
// UCAxMCTL - 8bit modulation control word register
//   UCBRFx - first modulation stage select (4 bit) | UCBRSx (3 bit) | UCOS16 (1 bit)

#if SMCLK_FREQ == 1000000

#define               UC_CTL  UCSSEL__SMCLK

#define         BR_9600_BAUD  104
#define       MCTL_9600_BAUD  UCBRS_1

#define        BR_19200_BAUD  52
#define      MCTL_19200_BAUD  0

#define        BR_38400_BAUD  26
#define      MCTL_38400_BAUD  0

#define        BR_57600_BAUD  17
#define      MCTL_57600_BAUD  UCBRS_3

#define       BR_115200_BAUD  8
#define     MCTL_115200_BAUD  UCBRS_6

#elif SMCLK_FREQ == 4000000

#define              UC_CTL1  UCSSEL__SMCLK

#define         BR_9600_BAUD  416
#define       MCTL_9600_BAUD  UCBRS_6

#define        BR_19200_BAUD  208
#define      MCTL_19200_BAUD  UCBRS_3

#define        BR_38400_BAUD  104
#define      MCTL_38400_BAUD  UCBRS_1

#define        BR_57600_BAUD  69
#define      MCTL_57600_BAUD  UCBRS_4

#define       BR_115200_BAUD  34
#define     MCTL_115200_BAUD  UCBRS_6


#elif SMCLK_FREQ == 8000000

#define              UC_CTL1  UCSSEL__SMCLK

#define         BR_9600_BAUD  833
#define       MCTL_9600_BAUD  UCBRS_2

#define        BR_19200_BAUD  416
#define      MCTL_19200_BAUD  UCBRS_6

#define        BR_38400_BAUD  208
#define      MCTL_38400_BAUD  UCBRS_3

#define        BR_57600_BAUD  138
#define      MCTL_57600_BAUD  UCBRS_7

#define       BR_115200_BAUD  69
#define     MCTL_115200_BAUD  UCBRS_4


#elif SMCLK_FREQ == 16000000

#define              UC_CTL1  UCSSEL__SMCLK

#define         BR_9600_BAUD  1666
#define       MCTL_9600_BAUD  UCBRS_6

#define        BR_19200_BAUD  833
#define      MCTL_19200_BAUD  UCBRS_2

#define        BR_38400_BAUD  416
#define      MCTL_38400_BAUD  UCBRS_6

#define        BR_57600_BAUD  277
#define      MCTL_57600_BAUD  UCBRS_7

#define       BR_115200_BAUD  138
#define     MCTL_115200_BAUD  UCBRS_7

#elif SMCLK_FREQ == 20000000

#define              UC_CTL1  UCSSEL__SMCLK

#define         BR_9600_BAUD  2083
#define       MCTL_9600_BAUD  UCBRS_2

#define        BR_19200_BAUD  1041
#define      MCTL_19200_BAUD  UCBRS_6

#define        BR_38400_BAUD  520
#define      MCTL_38400_BAUD  UCBRS_7

#define        BR_57600_BAUD  347
#define      MCTL_57600_BAUD  UCBRS_2

#define       BR_115200_BAUD  173
#define     MCTL_115200_BAUD  UCBRS_5

#elif SMCLK_FREQ == 24000000

#define              UC_CTL1  UCSSEL__SMCLK

#define         BR_9600_BAUD  
#define       MCTL_9600_BAUD  

#define        BR_19200_BAUD  
#define      MCTL_19200_BAUD  

#define        BR_38400_BAUD  
#define      MCTL_38400_BAUD  

#define        BR_57600_BAUD  
#define      MCTL_57600_BAUD  

#define       BR_115200_BAUD  
#define     MCTL_115200_BAUD  

#else
    #error SMCLK_FREQ is not defined or sourced from clock.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

uint8_t uart_config_reg(const uint16_t baseAddress, const uint8_t baudrate);

#ifdef __cplusplus
}
#endif

#endif
