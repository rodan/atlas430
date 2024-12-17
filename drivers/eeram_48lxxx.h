#ifndef __EERAM_48LXXX_H__
#define __EERAM_48LXXX_H__

/**
* \addtogroup MOD_DRIVERS Microchip 48L512/48LM01 serial EERAM driver
* \brief driver for the Microchip 48L512/48LM01 serial EERAM driver
    \note the CS signal is managed by the library, but make sure the spi_descriptor is populated correctly
    \note during write transfers, there is no need to set the most significat bit in the address byte to one, the library takes care of it
* \author Petre Rodan
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_DRIVERS
* \author Petre Rodan
**/

// opcodes
#define       EERAM_48L512_WREN  0x06 ///< Set Write Enable Latch (WEL)
#define       EERAM_48L512_WRDI  0x04 ///< Reset Write Enable Latch (WEL)
#define      EERAM_48L512_WRITE  0x02 ///< Write to SRAM Array
#define       EERAM_48L512_READ  0x03 ///< Read from SRAM Array
#define     EERAM_48L512_SWRITE  0x12 ///< Secure Write to SRAM Array with CRC
#define      EERAM_48L512_SREAD  0x13 ///< Secure Read from SRAM Array with CRC
#define       EERAM_48L512_WRSR  0x01 ///< Write STATUS Register (SR)
#define       EERAM_48L512_RDSR  0x05 ///< Read STATUS Register (SR)
#define      EERAM_48L512_STORE  0x08 ///< Store SRAM data to EEPROM array
#define     EERAM_48L512_RECALL  0x09 ///< Copy EEPROM data to SRAM array
#define      EERAM_48L512_WRNUR  0xc2 ///< Write Nonvolatile User Space
#define      EERAM_48L512_RDNUR  0xc3 ///< Read Nonvolatile User Space
#define  EERAM_48L512_HIBERNATE  0xb9 ///< Enter Hibernate Mode

// status register flags
#define     EERAM_48L512_SR_BSY  0x01 ///< busy bit (Read-Only)
#define     EERAM_48L512_SR_WEL  0x02 ///< Write Enable Latch bit (Read-Only)
#define     EERAM_48L512_SR_BP0  0x04 ///< Block Protection bit0
#define     EERAM_48L512_SR_BP1  0x08 ///< Block Protection bit1
#define     EERAM_48L512_SR_SWM  0x10 ///< Secure Write Monitoring bit (Read-Only)
#define     EERAM_48L512_SR_ASE  0x40 ///< AutoStore Enable bit

#ifdef __cplusplus
extern "C" {
#endif

/** read count number of bytes from the eeram chip
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param addr  starting address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param buf   pre-allocated buffer that holds the data. must be at least count bytes in length
    @param count number of bytes to be transfered
    @return non-zero on failure
*/
uint8_t EERAM_48L_read(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count);

/** write count number of bytes to the eeram chip
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param addr  starting address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param buf   pre-allocated buffer that will hold the data. must be at least count bytes in length
    @param count number of bytes to be transfered
    @return non-zero on failure
*/
uint8_t EERAM_48L_write(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count);

/** read the status register from the eeram chip
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param vat   uint8_t container for the read value
    @return non-zero on failure
*/
uint8_t EERAM_48L_read_streg(const spi_descriptor *spid, uint8_t *val);

/** write the status register to the eeram chip
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param vat   byte that will get written
    @return non-zero on failure
*/
uint8_t EERAM_48L_write_streg(const spi_descriptor *spid, const uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
///\}
