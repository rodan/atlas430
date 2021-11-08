#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define        BAUDRATE_9600  0x1
#define       BAUDRATE_19200  0x2
#define       BAUDRATE_38400  0x3
#define       BAUDRATE_57600  0x4
#define      BAUDRATE_115200  0x5

#define        UART_RET_WAKE  1
#define       UART_RET_SLEEP  0

#define      UART0_RX_NO_ERR  0x0
#define   UART0_RX_WAKE_MAIN  0x1 // ringbuffer got a special value, wake up main loop
#define      UART0_RX_ERR_RB  0x2 // ringbuffer is full, cannot add new element

// values based on slau367p.pdf table 30-5 Typical Crystals and Baud Rates

// UCAxBRW.UCBRx - clock prescaler (16bit)
// UCAxMCTLW - 16bit modulation control word register (16bit)
//                     UCBRSx - second modulation stage select - 8bit
// UCBRFX - first modulation stage select - 4 bit | reserved - 3 bit | UCOS16 - 1 bit

#if SMCLK_FREQ == 1000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        -0.48  0.64  -1.04  1.04
#define        BRW_9600_BAUD  6
#define      MCTLW_9600_BAUD  0x2081

// err                        -0.8  0.96  -1.84  1.84
#define       BRW_19200_BAUD  3
#define     MCTLW_19200_BAUD  0x2041

// err                        0  1.76  0  3.44
#define       BRW_38400_BAUD  1
#define     MCTLW_38400_BAUD  0x00a1

// err                        -2.72  2.56  -3.76  7.28
#define       BRW_57600_BAUD  17
#define     MCTLW_57600_BAUD  0x4a00

// err                        -7.36  5.6  -17.04  6.96
#define      BRW_115200_BAUD  8
#define    MCTLW_115200_BAUD  0xd600


#elif SMCLK_FREQ == 4000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        -0.08  0.16  -0.28  0.2
#define        BRW_9600_BAUD  26
#define      MCTLW_9600_BAUD  0xb601

// err                        -0.32  0.32  -0.64  0.48
#define       BRW_19200_BAUD  13
#define     MCTLW_19200_BAUD  0x8401

// err                        -0.48  0.64  -1.04  1.04
#define       BRW_38400_BAUD  6
#define     MCTLW_38400_BAUD  0x2081

// err                        -0.8  0.64  -1.12  1.76
#define       BRW_57600_BAUD  4
#define     MCTLW_57600_BAUD  0x5551

// err                        -1.44  1.28  -3.92  1.68
#define      BRW_115200_BAUD  2
#define    MCTLW_115200_BAUD  0xbb21


#elif SMCLK_FREQ == 8000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        -0.08  0.04  -0.1  0.14
#define        BRW_9600_BAUD  52
#define      MCTLW_9600_BAUD  0x4911

// err                        -0.08  0.16  -0.28  0.2
#define       BRW_19200_BAUD  26
#define     MCTLW_19200_BAUD  0xb601

// err                        -0.32  0.32  -0.64  0.48
#define       BRW_38400_BAUD  13
#define     MCTLW_38400_BAUD  0x8401

// err                        -0.32  0.32  -1.00  0.36
#define       BRW_57600_BAUD  8
#define     MCTLW_57600_BAUD  0xf7a1

// err                        -0.80  0.64  -1.12  1.76
#define      BRW_115200_BAUD  4
#define    MCTLW_115200_BAUD  0x5551


#elif SMCLK_FREQ == 16000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        -0.04  0.02  -0.09  0.03
#define        BRW_9600_BAUD  104
#define      MCTLW_9600_BAUD  0xd621

// err                        -0.08  0.04  -0.10  0.14
#define       BRW_19200_BAUD  52
#define     MCTLW_19200_BAUD  0x4911

// err                        -0.08  0.16  -0.28  0.20
#define       BRW_38400_BAUD  26
#define     MCTLW_38400_BAUD  0xb601

// err                        -0.16  0.20  -0.30  0.38
#define       BRW_57600_BAUD  17
#define     MCTLW_57600_BAUD  0xdd51

// err                        -0.32  0.32  -1.00  0.36
#define      BRW_115200_BAUD  8
#define    MCTLW_115200_BAUD  0xf7a1

#else
    #error SMCLK_FREQ is not defined or sourced from clock.h
#endif

#ifdef __cplusplus
}
#endif

#endif
