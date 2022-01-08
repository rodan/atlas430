#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>
#include "uart_mapping.h"

#define true                1
#define false               0

/*!
	\brief List of possible message types for the event handler.
	\sa eh_register()
*/

#define           SYS_MSG_NULL 0
#define    SYS_MSG_UART_BCL_RX 0x1   // UART received something
#define        SYS_MSG_P53_INT 0x2   // ds3234 interrupt

/*
#if defined __MSP430_HAS_EUSCI_Ax__

#if defined SPI_USES_UCA0
    #define SPI_BASE_ADDR EUSCI_A0_BASE
#elif defined SPI_USES_UCA1
    #define SPI_BASE_ADDR EUSCI_A1_BASE
#elif defined SPI_USES_UCA2
    #define SPI_BASE_ADDR EUSCI_A2_BASE
#elif defined SPI_USES_UCA3
    #define SPI_BASE_ADDR EUSCI_A3_BASE
#endif

#elif defined __MSP430_HAS_EUSCI_Bx__

#if defined SPI_USES_UCB0
    #define SPI_BASE_ADDR EUSCI_B0_BASE
#elif defined SPI_USES_UCB1
    #define SPI_BASE_ADDR EUSCI_B1_BASE
#elif defined SPI_USES_UCB2
    #define SPI_BASE_ADDR EUSCI_B2_BASE
#elif defined SPI_USES_UCB3
    #define SPI_BASE_ADDR EUSCI_B3_BASE
#endif

#elif defined __MSP430_HAS_USCI_Ax__

#if defined SPI_USES_UCA0
    #define SPI_BASE_ADDR USCI_A0_BASE
#elif defined SPI_USES_UCA1
    #define SPI_BASE_ADDR USCI_A1_BASE
#elif defined SPI_USES_UCA2
    #define SPI_BASE_ADDR USCI_A2_BASE
#elif defined SPI_USES_UCA3
    #define SPI_BASE_ADDR USCI_A3_BASE
#endif

#elif defined __MSP430_HAS_USCI_Bx__

#if defined SPI_USES_UCB0
    #define SPI_BASE_ADDR USCI_B0_BASE
#elif defined SPI_USES_UCB1
    #define SPI_BASE_ADDR USCI_B1_BASE
#elif defined SPI_USES_UCB2
    #define SPI_BASE_ADDR USCI_B2_BASE
#elif defined SPI_USES_UCB3
    #define SPI_BASE_ADDR USCI_B3_BASE
#endif

#endif
*/

#endif
