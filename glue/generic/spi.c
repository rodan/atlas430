
#include <msp430.h>
#include <inttypes.h>
#include "driverlib.h"
#include "spi.h"

#if defined (__MSP430_HAS_EUSCI_Ax__) || defined (__MSP430_HAS_EUSCI_Bx__)
uint8_t spi_read_frame(const uint16_t baseAddress, uint8_t * pBuffer, uint16_t size)
{
    uint8_t rv = 0;
    // store current GIE state
    uint16_t gie = _get_SR_register() & GIE;

    // make this operation atomic
    __disable_interrupt();


    if (
#if defined EUSCI_A0_BASE
    (baseAddress == EUSCI_A0_BASE) ||
#endif
#ifdef EUSCI_A1_BASE
    (baseAddress == EUSCI_A1_BASE) ||
#endif
#ifdef EUSCI_A2_BASE
    (baseAddress == EUSCI_A2_BASE) ||
#endif
#ifdef EUSCI_A3_BASE
    (baseAddress == EUSCI_A3_BASE) ||
#endif
    0 ) { 
        // ensure RXIFG is clear
        EUSCI_A_SPI_clearInterrupt(baseAddress, UCRXIFG);

        // clock the actual data transfer and receive the bytes
        while (size--) {
            // wait while not ready for TX
            while (!(EUSCI_A_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
            // write dummy byte
            EUSCI_A_SPI_transmitData(baseAddress, 0xff);
            // wait for RX buffer (full)
            while (!(EUSCI_A_SPI_getInterruptStatus(baseAddress, UCRXIFG))) {};
            *pBuffer++ = EUSCI_A_SPI_receiveData(baseAddress);
        }
    } else {
        // ensure RXIFG is clear
        EUSCI_B_SPI_clearInterrupt(baseAddress, UCRXIFG);

        // clock the actual data transfer and receive the bytes
        while (size--) {
            __delay_cycles(SPI_DELAY);
            // wait while not ready for TX
            while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
            // write dummy byte
            EUSCI_B_SPI_transmitData(baseAddress, 0xff);
            // wait for RX buffer (full)
            __delay_cycles(SPI_DELAY);
            while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCRXIFG))) {};
            *pBuffer++ = EUSCI_B_SPI_receiveData(baseAddress);
        }
    }

    // restore original GIE state
    _bis_SR_register(gie);

    return rv;
}

uint8_t spi_write_frame(const uint16_t baseAddress, uint8_t * pBuffer, uint16_t size)
{
    uint8_t rv = 0;

    // store current GIE state
    uint16_t gie = _get_SR_register() & GIE;

    // make this operation atomic
    __disable_interrupt();

    if (
#if defined EUSCI_A0_BASE
    (baseAddress == EUSCI_A0_BASE) ||
#endif
#ifdef EUSCI_A1_BASE
    (baseAddress == EUSCI_A1_BASE) ||
#endif
#ifdef EUSCI_A2_BASE
    (baseAddress == EUSCI_A2_BASE) ||
#endif
#ifdef EUSCI_A3_BASE
    (baseAddress == EUSCI_A3_BASE) ||
#endif
    0 ) {
        // clock the actual data transfer and send the bytes. Note that we
        // intentionally not read out the receive buffer during frame transmission
        // in order to optimize transfer speed, however we need to take care of the
        // resulting overrun condition.
        while (size--) {
            // wait while not ready for TX
            while (!(EUSCI_A_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
            EUSCI_A_SPI_transmitData(baseAddress, *pBuffer++);
        }
        while (EUSCI_A_SPI_isBusy(baseAddress)) {};

        // dummy read to empty RX buffer
        // and clear any overrun conditions
        EUSCI_A_SPI_receiveData(baseAddress);

    } else {
        // clock the actual data transfer and send the bytes. Note that we
        // intentionally not read out the receive buffer during frame transmission
        // in order to optimize transfer speed, however we need to take care of the
        // resulting overrun condition.
        while (size--) {
            // wait while not ready for TX
            __delay_cycles(SPI_DELAY);
            while (!(EUSCI_B_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
            EUSCI_B_SPI_transmitData(baseAddress, *pBuffer++);
            //__delay_cycles(SPI_DELAY); // dragons
        }
        while (EUSCI_B_SPI_isBusy(baseAddress)) {};

        // dummy read to empty RX buffer
        // and clear any overrun conditions
        EUSCI_B_SPI_receiveData(baseAddress);
    }

    // restore original GIE state
    _bis_SR_register(gie);

    return rv;
}

#elif defined (__MSP430_HAS_USCI_Ax__) || defined (__MSP430_HAS_USCI_Bx__)

uint8_t spi_read_frame(const uint16_t baseAddress, uint8_t * pBuffer, uint16_t size)
{
    uint8_t rv = 0;
    // store current GIE state
    uint16_t gie = _get_SR_register() & GIE;

    // make this operation atomic
    __disable_interrupt();

    if (
#if defined USCI_A0_BASE
    (baseAddress == USCI_A0_BASE) ||
#endif
#ifdef USCI_A1_BASE
    (baseAddress == USCI_A1_BASE) ||
#endif
#ifdef USCI_A2_BASE
    (baseAddress == USCI_A2_BASE) ||
#endif
#ifdef USCI_A3_BASE
    (baseAddress == USCI_A3_BASE) ||
#endif
    0 ) { 
        // ensure RXIFG is clear
        USCI_A_SPI_clearInterrupt(baseAddress, UCRXIFG);

        // clock the actual data transfer and receive the bytes
        while (size--) {
            // wait while not ready for TX
            while (!(USCI_A_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
            // write dummy byte
            USCI_A_SPI_transmitData(baseAddress, 0xff);
            // wait for RX buffer (full)
            while (!(USCI_A_SPI_getInterruptStatus(baseAddress, UCRXIFG))) {};
            *pBuffer++ = USCI_A_SPI_receiveData(baseAddress);
        }
    } else {
        // ensure RXIFG is clear
        USCI_B_SPI_clearInterrupt(baseAddress, UCRXIFG);

        // clock the actual data transfer and receive the bytes
        while (size--) {
            // wait while not ready for TX
            while (!(USCI_B_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
            // write dummy byte
            USCI_B_SPI_transmitData(baseAddress, 0xff);
            // wait for RX buffer (full)
            while (!(USCI_B_SPI_getInterruptStatus(baseAddress, UCRXIFG))) {};
            *pBuffer++ = USCI_B_SPI_receiveData(baseAddress);
        }
    }

    // restore original GIE state
    _bis_SR_register(gie);

    return rv;
}

uint8_t spi_write_frame(const uint16_t baseAddress, uint8_t * pBuffer, uint16_t size)
{
    uint8_t rv = 0;

    // store current GIE state
    uint16_t gie = _get_SR_register() & GIE;

    // make this operation atomic
    __disable_interrupt();

    if (
#if defined USCI_A0_BASE
    (baseAddress == USCI_A0_BASE) ||
#endif
#ifdef USCI_A1_BASE
    (baseAddress == USCI_A1_BASE) ||
#endif
#ifdef USCI_A2_BASE
    (baseAddress == USCI_A2_BASE) ||
#endif
#ifdef USCI_A3_BASE
    (baseAddress == USCI_A3_BASE) ||
#endif
    0 ) { 
        // clock the actual data transfer and send the bytes. Note that we
        // intentionally not read out the receive buffer during frame transmission
        // in order to optimize transfer speed, however we need to take care of the
        // resulting overrun condition.
        while (size--) {
            // wait while not ready for TX
            while (!(USCI_A_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
            USCI_A_SPI_transmitData(baseAddress, *pBuffer++);
        }
        while (USCI_A_SPI_isBusy(baseAddress)) {};

        // dummy read to empty RX buffer
        // and clear any overrun conditions
        USCI_A_SPI_receiveData(baseAddress);
    } else {
        // clock the actual data transfer and send the bytes. Note that we
        // intentionally not read out the receive buffer during frame transmission
        // in order to optimize transfer speed, however we need to take care of the
        // resulting overrun condition.
        while (size--) {
            // wait while not ready for TX
            while (!(USCI_B_SPI_getInterruptStatus(baseAddress, UCTXIFG))) {};
            USCI_B_SPI_transmitData(baseAddress, *pBuffer++);
        }
        while (USCI_B_SPI_isBusy(baseAddress)) {};

        // dummy read to empty RX buffer
        // and clear any overrun conditions
        USCI_B_SPI_receiveData(baseAddress);
    }

    // restore original GIE state
    _bis_SR_register(gie);

    return rv;
}

#endif
