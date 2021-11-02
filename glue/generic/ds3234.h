#ifndef __DS3234_H_
#define __DS3234_H_

/**
* \addtogroup MOD_DRIVERS Maxim DS3234 IC driver
* \brief driver for the Maxim DS3234 RTC chip
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


//! DS3234 registers
#define            DS3234_REG_RTC  0x00 ///< real time clock seconds
#define             DS3234_REG_A1  0x07 ///< alarm1 seconds
#define             DS3234_REG_A2  0x0b ///< alarm2 minutes
#define           DS3234_REG_CTRL  0x0e ///< control register
#define            DS3234_REG_STA  0x0f ///< control/status register
#define          DS3234_REG_AGING  0x10 ///< crystal aging offset
#define           DS3234_REG_TEMP  0x11 ///< temperature
#define           DS3234_REG_CONV  0x13 ///< disable temp conversions
#define      DS3234_REG_SRAM_ADDR  0x18 ///< SRAM address
#define      DS3234_REG_SRAM_DATA  0x19 ///< SRAM data

//! control register 0x0e
#define               DS3234_A1IE  0x01 ///< Alarm1 interrupt enable (1 to enable)
#define               DS3234_A2IE  0x02 ///< Alarm2 interrupt enable (1 to enable)
#define              DS3234_INTCN  0x04 ///< Interrupt control (1 for use of the alarms and to disable square wave)
#define                DS3234_RS1  0x08 ///< Rate select - frequency of square wave output
#define                DS3234_RS2  0x10 ///< Rate select - frequency of square wave output
#define               DS3234_CONV  0x20 ///< Convert temperature (1 forces a new conversion)
#define              DS3234_BBSQW  0x40 ///< Battery Backed Square Wave
#define               DS3234_EOSC  0x80 ///< Enable Oscillator (1 if oscillator must be stopped when on battery)

//! control/status register 0x0f
#define                DS3234_A1F  0x01 ///< Alarm 1 Flag - (1 if alarm1 was triggered)
#define                DS3234_A2F  0x02 ///< Alarm 2 Flag - (1 if alarm2 was triggered)
#define                DS3234_BSY  0x04 ///< Busy with TCXO functions
#define            DS3234_EN32KHZ  0x08 ///< Enable 32kHz output (1 if needed)
#define             DS3234_CRATE0  0x10 ///< Conversion rate 0  temperature compensation rate
#define             DS3234_CRATE1  0x20 ///< Conversion rate 1  temperature compensation rate
#define            DS3234_BB32KHZ  0x40 ///< Battery Backed 32kHz output (1 if square wave is needed when powered by battery)
#define                DS3234_OSF  0x80 ///< Oscillator Stop Flag (if 1 then oscillator has stopped and date might be innacurate)

///< alarm1 flags
#define         DS3234_A1_NOMATCH  0x0f ///< trigger alarm every second, nothing needs to match
#define         DS3234_A1_S_MATCH  0x0e ///< alarm when seconds match
#define        DS3234_A1_SM_MATCH  0x0c ///< alarm when seconds and minutes match
#define       DS3234_A1_SMH_MATCH  0x08 ///< alarm when seconds, minutes and hours match
#define     DS3234_A1_SMHDm_MATCH  0x00 ///< alarm when seconds, minutes, hours and day of month  match
#define     DS3234_A1_SMHDw_MATCH  0x10 ///< alarm when seconds, minutes, hours and day of week match

///< alarm2 flags
#define         DS3234_A2_NOMATCH  0x07 ///< alarm once every minute at 0 seconds of each minute
#define         DS3234_A2_M_MATCH  0x06 ///< alarm when minutes match
#define        DS3234_A2_MH_MATCH  0x04 ///< alarm when minutes and hours match
#define      DS3234_A2_MHDm_MATCH  0x00 ///< alarm when minutes, hours and day of month match
#define      DS3234_A2_MHDw_MATCH  0x08 ///< alarm when minutes, hours and day of week match

#ifdef __cplusplus
extern "C" {
#endif

/** read count number of bytes from the ds3234
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param addr  starting address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param buf   pre-allocated buffer that holds the data. must be at least count bytes in length
    @param count number of bytes to be transfered
    @return non-zero on failure
*/
uint8_t DS3234_read(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count);

/** write count number of bytes to the ds3234
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param addr  starting address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param buf   pre-allocated buffer that will hold the data. must be at least count bytes in length
    @param count number of bytes to be transfered
    @return non-zero on failure
*/
uint8_t DS3234_write(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count);

/** read one register from the ds3234
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param addr  register address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param vat   uint8_t container for the read value
    @return non-zero on failure
*/
uint8_t DS3234_read_reg(const spi_descriptor *spid, const uint8_t addr, uint8_t *val);

/** write one register to the ds3234
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param addr  register address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param vat   byte that will get written
    @return non-zero on failure
*/
uint8_t DS3234_write_reg(const spi_descriptor *spid, const uint8_t addr, const uint8_t val);

/** read date and time from the IC
    @param spid   descriptor containing the SPI subsystem base address and CS signal control
    @param t      date and time struct
    @return non-zero on failure
*/
uint8_t DS3234_read_rtc(const spi_descriptor *spid, struct ts *t);

/** write date and time to the IC
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param t     date and time struct
    @return non-zero on failure
*/
uint8_t DS3234_write_rtc(const spi_descriptor *spid, struct ts *t);

/** read alarm A1 registers
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param t     date and time struct. only the second, minute, hour, day is used from the struct
    @param flags what time element to trigger on
    @return non-zero on failure
*/
uint8_t DS3234_read_a1(const spi_descriptor *spid, struct ts *t, uint8_t *flags);

/** write to alarm A1 registers
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param t     date and time struct. only the second, minute, hour, day is used from the struct
    @param flags what time element to trigger on
    @return non-zero on failure
*/
uint8_t DS3234_write_a1(const spi_descriptor *spid, const struct ts *t, const uint8_t flags);

/** read alarm A2 registers
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param t     date and time struct. only the minute, hour, day is used from the struct
    @param flags what time element to trigger on
    @return non-zero on failure
*/
uint8_t DS3234_read_a2(const spi_descriptor *spid, struct ts *t, uint8_t *flags);

/** write to alarm A1 registers
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param t     date and time struct. only the second, minute, hour, day is used from the struct
    @param flags what time element to trigger on
    @return non-zero on failure
*/
uint8_t DS3234_write_a2(const spi_descriptor *spid, const struct ts *t, const uint8_t flags);

/** read and convert the aging register from the IC
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param value crystal aging register value
    @return non-zero on failure
*/
uint8_t DS3234_read_aging(const spi_descriptor *spid, int8_t *value);

/** convert and write the aging register to the IC
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param value crystal aging value
    @return non-zero on failure
*/
uint8_t DS3234_write_aging(const spi_descriptor *spid, const int8_t value);

/** read the temperature sensor from within the IC
    @param spid descriptor containing the SPI subsystem base address and CS signal control
    @param value temperature in degrees C multiplied by 100 - in int16_t form
    @return non-zero on failure
*/
uint8_t DS3234_read_temp(const spi_descriptor *spid, int16_t *value);

/** read count number of bytes from the SRAM of ds3234
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param addr  starting SRAM relative address [0x0 - 0xff]
    @param buf   pre-allocated buffer that will hold the data. must be at least count bytes in length
    @param count number of bytes to be transfered
    @return non-zero on failure
*/
uint8_t DS3234_read_sram(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count);

/** write count number of bytes to the SRAM of ds3234
    @param spid  descriptor containing the SPI subsystem base address and CS signal control
    @param addr  starting SRAM relative address [0x0 - 0xff]
    @param buf   buffer to be written. must be at least count bytes in length
    @param count number of bytes to be transfered
    @return non-zero on failure
*/
uint8_t DS3234_write_sram(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count);

#ifdef __cplusplus
}
#endif

#endif
///\}
