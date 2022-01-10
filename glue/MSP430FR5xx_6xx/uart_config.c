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
    HWREG16(baseAddress + OFS_UCAxCTLW0) |= UCSWRST;

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

    HWREG16(baseAddress + OFS_UCAxCTLW0) &= ~UCSSEL_3;

    switch (baudrate) {
        case BAUDRATE_9600:
            HWREG16(baseAddress + OFS_UCAxCTLW0) |= UC_CTLW0;
            HWREG16(baseAddress + OFS_UCAxBRW) = BRW_9600_BAUD;
            HWREG16(baseAddress + OFS_UCAxMCTLW) = MCTLW_9600_BAUD;
            break;
        case BAUDRATE_19200:
            HWREG16(baseAddress + OFS_UCAxCTLW0) |= UC_CTLW0;
            HWREG16(baseAddress + OFS_UCAxBRW) = BRW_19200_BAUD;
            HWREG16(baseAddress + OFS_UCAxMCTLW) = MCTLW_19200_BAUD;
            break;
        case BAUDRATE_38400:
            HWREG16(baseAddress + OFS_UCAxCTLW0) |= UC_CTLW0;
            HWREG16(baseAddress + OFS_UCAxBRW) = BRW_38400_BAUD;
            HWREG16(baseAddress + OFS_UCAxMCTLW) = MCTLW_38400_BAUD;
            break;
        case BAUDRATE_57600:
            HWREG16(baseAddress + OFS_UCAxCTLW0) |= UC_CTLW0;
            HWREG16(baseAddress + OFS_UCAxBRW) = BRW_57600_BAUD;
            HWREG16(baseAddress + OFS_UCAxMCTLW) = MCTLW_57600_BAUD;
            break;
        case BAUDRATE_115200:
            HWREG16(baseAddress + OFS_UCAxCTLW0) |= UC_CTLW0;
            HWREG16(baseAddress + OFS_UCAxBRW) = BRW_115200_BAUD;
            HWREG16(baseAddress + OFS_UCAxMCTLW) = MCTLW_115200_BAUD;
            break;
    }

    // initialize USCI
    HWREG16(baseAddress + OFS_UCAxCTLW0) &= ~UCSWRST;

#ifdef UART_TX_USES_IRQ
    // enable RX and TX interrupts
    HWREG16(uartd->baseAddress + OFS_UCAxIE) |= UCRXIE | UCTXIE;
#else
    // enable only the RX interrupt
    HWREG16(uartd->baseAddress + OFS_UCAxIE) |= UCRXIE;
#endif

}

