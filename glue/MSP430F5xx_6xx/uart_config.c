/*
  uart register initialization function
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430
*/

#include <msp430.h>
#include <inttypes.h>
#include "inc/hw_memmap.h"
#include "clock_selection.h"
#include "uart_config.h"

void uart_config_reg(const uint16_t baseAddress, const uint8_t baudrate)
{
    //put USCI state machine in reset
    HWREG8(baseAddress + OFS_UCAxCTL1) |= UCSWRST;

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

    switch (baudrate) {
        case BAUDRATE_9600:
            HWREG8(baseAddress + OFS_UCAxCTL1) |= UC_CTL1;
            HWREG16(baseAddress + OFS_UCAxBRW) = BR_9600_BAUD;
            HWREG8(baseAddress + OFS_UCAxMCTL) = MCTL_9600_BAUD;
            break;
        case BAUDRATE_19200:
            HWREG8(baseAddress + OFS_UCAxCTL1) |= UC_CTL1;
            HWREG16(baseAddress + OFS_UCAxBRW) = BR_19200_BAUD;
            HWREG8(baseAddress + OFS_UCAxMCTL) = MCTL_19200_BAUD;
            break;
        case BAUDRATE_38400:
            HWREG8(baseAddress + OFS_UCAxCTL1) |= UC_CTL1;
            HWREG16(baseAddress + OFS_UCAxBRW) = BR_38400_BAUD;
            HWREG8(baseAddress + OFS_UCAxMCTL) = MCTL_38400_BAUD;
            break;
        case BAUDRATE_57600:
            HWREG8(baseAddress + OFS_UCAxCTL1) |= UC_CTL1;
            HWREG16(baseAddress + OFS_UCAxBRW) = BR_57600_BAUD;
            HWREG8(baseAddress + OFS_UCAxMCTL) = MCTL_57600_BAUD;
            break;
        case BAUDRATE_115200:
            HWREG8(baseAddress + OFS_UCAxCTL1) |= UC_CTL1;
            HWREG16(baseAddress + OFS_UCAxBRW) = BR_115200_BAUD;
            HWREG8(baseAddress + OFS_UCAxMCTL) = MCTL_115200_BAUD;
            break;
    }

    // initialize USCI
    HWREG8(baseAddress + OFS_UCAxCTL1) &= ~UCSWRST;

#ifdef UART_TX_USES_IRQ
    // enable RX and TX interrupts
    HWREG8(uartd->baseAddress + OFS_UCAxIE) |= UCRXIE | UCTXIE;
#else
    // enable only the RX interrupt
    HWREG8(uartd->baseAddress + OFS_UCAxIE) |= UCRXIE;
#endif
}

