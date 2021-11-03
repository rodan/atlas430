#ifndef __SPI_H__
#define __SPI_H__

/**
* \addtogroup MOD_SPI SPI Bus
* \brief Application-level SPI driver.
* \author Petre Rodan
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_SPI
* \author Petre Rodan
**/

typedef struct _spi_descriptor {
    uint16_t baseAddress;   ///< SPI subsystem offset base register
    void (*cs_low)();       ///< pointer to function that brings the CS signal low
    void (*cs_high)();      ///< pointer to function that brings the CS signal high
    void (*spi_init)();     ///< pointer to function that sets up the spi pins
    void (*spi_deinit)();   ///< pointer to function that sets the spi pins to a HIGH-Z state
    uint8_t (*dev_is_busy)(); ///< pointer to function that returns true if the device is busy, false otherwise
} spi_descriptor; ///< struct that needs to be populated before the library calls are made

#ifdef __cplusplus
extern "C" {
#endif

/** \brief read count number of bytes from the SPI

    this function starts a new SPI read transaction as described by the provided parameters.

    \note the CS signal is managed by the function calling spi_read_frame.

    @param baseAddress an MSP430-specific register address that marks the offset for the currently used SPI subsystem
    @param buf pre-allocated buffer that will hold the information
    @param count  number of bytes to be transfered
    @return non-zero on failure
*/
uint8_t spi_read_frame(const uint16_t baseAddress, uint8_t *buf, const uint16_t count);

/** \brief write count number of bytes to the SPI

    this function starts a new SPI write transaction as described by the provided parameters.

    \note the CS signal is managed by the function calling spi_read_frame.

    @param baseAddress msp430-specific register address that marks the offset for the currently used SPI subsystem
    @param buf pre-allocated buffer that will hold the information
    @param count  number of bytes to be transfered
    @return non-zero on failure
*/
uint8_t spi_write_frame(const uint16_t baseAddress, uint8_t *buf, const uint16_t count);

#ifdef __cplusplus
}
#endif

#endif
///\}
