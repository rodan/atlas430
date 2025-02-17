#ifndef __DSRTC_H_
#define __DSRTC_H_

/**
* \addtogroup MOD_DRIVERS Maxim DS3231/DS3234 IC driver
* \brief driver for the Maxim DS3231/DS3234 RTC chips
    \note the CS signal is managed by the library, but make sure the spi_descriptor is populated correctly
    \note during write transfers, there is no need to set the most significat bit in the address byte to one, the library takes care of it
* \author Petre Rodan
* 
* \{
**/

// i2c slave address of the DS3231 chip
#define           DSRTC_I2C_ADDR  0x68

#define        DSRTC_TYPE_DS3231  0xcaf
#define        DSRTC_TYPE_DS3234  0xca2

//! DS3234 registers
#define            DSRTC_REG_RTC  0x00 ///< real time clock seconds
#define             DSRTC_REG_A1  0x07 ///< alarm1 seconds
#define             DSRTC_REG_A2  0x0b ///< alarm2 minutes
#define           DSRTC_REG_CTRL  0x0e ///< control register
#define            DSRTC_REG_STA  0x0f ///< control/status register
#define          DSRTC_REG_AGING  0x10 ///< crystal aging offset
#define           DSRTC_REG_TEMP  0x11 ///< temperature
#define           DSRTC_REG_CONV  0x13 ///< disable temp conversions
#define      DSRTC_REG_SRAM_ADDR  0x18 ///< SRAM address
#define      DSRTC_REG_SRAM_DATA  0x19 ///< SRAM data

//! control register 0x0e
#define               DSRTC_A1IE  0x01 ///< Alarm1 interrupt enable (1 to enable)
#define               DSRTC_A2IE  0x02 ///< Alarm2 interrupt enable (1 to enable)
#define              DSRTC_INTCN  0x04 ///< Interrupt control (1 for use of the alarms and to disable square wave)
#define                DSRTC_RS1  0x08 ///< Rate select - frequency of square wave output
#define                DSRTC_RS2  0x10 ///< Rate select - frequency of square wave output
#define               DSRTC_CONV  0x20 ///< Convert temperature (1 forces a new conversion)
#define              DSRTC_BBSQW  0x40 ///< Battery Backed Square Wave
#define               DSRTC_EOSC  0x80 ///< Enable Oscillator (1 if oscillator must be stopped when on battery)

//! control/status register 0x0f
#define                DSRTC_A1F  0x01 ///< Alarm 1 Flag - (1 if alarm1 was triggered)
#define                DSRTC_A2F  0x02 ///< Alarm 2 Flag - (1 if alarm2 was triggered)
#define                DSRTC_BSY  0x04 ///< Busy with TCXO functions
#define            DSRTC_EN32KHZ  0x08 ///< Enable 32kHz output (1 if needed)
#define             DSRTC_CRATE0  0x10 ///< Conversion rate 0  temperature compensation rate
#define             DSRTC_CRATE1  0x20 ///< Conversion rate 1  temperature compensation rate
#define            DSRTC_BB32KHZ  0x40 ///< Battery Backed 32kHz output (1 if square wave is needed when powered by battery)
#define                DSRTC_OSF  0x80 ///< Oscillator Stop Flag (if 1 then oscillator has stopped and date might be innacurate)

///< alarm1 flags
#define         DSRTC_A1_NOMATCH  0x0f ///< trigger alarm every second, nothing needs to match
#define         DSRTC_A1_S_MATCH  0x0e ///< alarm when seconds match
#define        DSRTC_A1_SM_MATCH  0x0c ///< alarm when seconds and minutes match
#define       DSRTC_A1_SMH_MATCH  0x08 ///< alarm when seconds, minutes and hours match
#define     DSRTC_A1_SMHDm_MATCH  0x00 ///< alarm when seconds, minutes, hours and day of month  match
#define     DSRTC_A1_SMHDw_MATCH  0x10 ///< alarm when seconds, minutes, hours and day of week match

///< alarm2 flags
#define         DSRTC_A2_NOMATCH  0x07 ///< alarm once every minute at 0 seconds of each minute
#define         DSRTC_A2_M_MATCH  0x06 ///< alarm when minutes match
#define        DSRTC_A2_MH_MATCH  0x04 ///< alarm when minutes and hours match
#define      DSRTC_A2_MHDm_MATCH  0x00 ///< alarm when minutes, hours and day of month match
#define      DSRTC_A2_MHDw_MATCH  0x08 ///< alarm when minutes, hours and day of week match

/* control register 0Eh/8Eh
bit7 EOSC   Enable Oscillator (1 if oscillator must be stopped when on battery)
bit6 BBSQW  Battery Backed Square Wave
bit5 CONV   Convert temperature (1 forces a conversion NOW)
bit4 RS2    Rate select - frequency of square wave output
bit3 RS1    Rate select
bit2 INTCN  Interrupt control (1 for use of the alarms and to disable square wave)
bit1 A2IE   Alarm2 interrupt enable (1 to enable)
bit0 A1IE   Alarm1 interrupt enable (1 to enable)
*/

/* status register
bit7 OSF      Oscillator Stop Flag (if 1 then oscillator has stopped and date might be innacurate)
bit3 EN32kHz  Enable 32kHz output (1 if needed)
bit2 BSY      Busy with TCXO functions
bit1 A2F      Alarm 2 Flag - (1 if alarm2 was triggered)
bit0 A1F      Alarm 1 Flag - (1 if alarm1 was triggered)
*/

typedef struct dsrtc_priv {
    uint16_t ic_type;
} dsrtc_priv_t;

#ifdef __cplusplus
extern "C" {
#endif

/** read a number of bytes of data from the rtc IC
    @param dev    device pointer
    @param offset starting address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param buf    pre-allocated buffer that holds the data. must be at least nbytes bytes in length
    @param nbytes number of bytes to be transfered
    @return non-zero on failure
*/
uint16_t dsrtc_read(device_t *dev, const uint8_t offset, uint8_t *buf, const uint8_t nbytes);

/** write a number of bytes of data to the rtc IC
    @param dev    device pointer
    @param offset starting address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param buf    pre-allocated buffer that will hold the data. must be at least nbytes bytes in length
    @param nbytes number of bytes to be transfered
    @return non-zero on failure
*/
uint16_t dsrtc_write(device_t *dev, const uint8_t offset, uint8_t *buf, const uint8_t nbytes);

/** read one register
    @param dev   device pointer
    @param addr  register address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param vat   uint8_t container for the read value
    @return non-zero on failure
*/
uint16_t dsrtc_read_reg(device_t *dev, const uint8_t addr, uint8_t *val);

/** write one register
    @param dev   device pointer
    @param addr  register address - use the _read_ address from the datasheet [0x0 - 0x19]
    @param vat   byte that will get written
    @return non-zero on failure
*/
uint16_t dsrtc_write_reg(device_t *dev, const uint8_t addr, const uint8_t val);

/** read date and time from the IC
    @param dev   device pointer
    @param t     date and time struct
    @return non-zero on failure
*/
uint16_t dsrtc_read_rtc(device_t *dev, struct ts *t);

/** write date and time to the IC
    @param dev   device pointer
    @param t     date and time struct
    @return non-zero on failure
*/
uint16_t dsrtc_write_rtc(device_t *dev, struct ts *t);

/** read alarm A1 registers
    @param dev   device pointer
    @param t     date and time struct. only the second, minute, hour, day is used from the struct
    @param flags what time element to trigger on
    @return non-zero on failure
*/
uint16_t dsrtc_read_a1(device_t *dev, struct ts *t, uint8_t *flags);

/** write to alarm A1 registers
    @param dev   device pointer
    @param t     date and time struct. only the second, minute, hour, day is used from the struct
    @param flags what time element to trigger on
    @return non-zero on failure
*/
uint16_t dsrtc_write_a1(device_t *dev, const struct ts *t, const uint8_t flags);

/** read alarm A2 registers
    @param dev   device pointer
    @param t     date and time struct. only the minute, hour, day is used from the struct
    @param flags what time element to trigger on
    @return non-zero on failure
*/
uint16_t dsrtc_read_a2(device_t *dev, struct ts *t, uint8_t *flags);

/** write to alarm A1 registers
    @param dev   device pointer
    @param t     date and time struct. only the second, minute, hour, day is used from the struct
    @param flags what time element to trigger on
    @return non-zero on failure
*/
uint16_t dsrtc_write_a2(device_t *dev, const struct ts *t, const uint8_t flags);

/** read and convert the aging register from the IC
    @param dev   device pointer
    @param value crystal aging register value
    @return non-zero on failure
*/
uint16_t dsrtc_read_aging(device_t *dev, int8_t *value);

/** convert and write the aging register to the IC
    @param dev   device pointer
    @param value crystal aging value
    @return non-zero on failure
*/
uint16_t dsrtc_write_aging(device_t *dev, const int8_t value);

/** read the temperature sensor from within the IC
    @param dev   device pointer
    @param value temperature in degrees C multiplied by 100 - in int16_t form
    @return non-zero on failure
*/
uint16_t dsrtc_read_temp(device_t *dev, int16_t *value);

/** read nbytes number of bytes from the SRAM of the IC (supported only by some ICs)
    @param dev    device pointer
    @param offset starting SRAM relative address [0x0 - 0xff]
    @param buf    pre-allocated buffer that will hold the data. must be at least nbytes bytes in length
    @param nbytes number of bytes to be transfered
    @return non-zero on failure
*/
uint16_t dsrtc_read_sram(device_t *dev, const uint8_t offset, uint8_t *buf, const uint8_t nbytes);

/** write nbytes number of bytes to the SRAM of the IC (supported only by some ICs)
    @param dev    device pointer
    @param offset starting SRAM relative address [0x0 - 0xff]
    @param buf    buffer to be written. must be at least nbytes bytes in length
    @param nbytes number of bytes to be transfered
    @return non-zero on failure
*/
uint16_t dsrtc_write_sram(device_t *dev, const uint8_t offset, uint8_t *buf, const uint8_t nbytes);

#if 0
uint8_t dsrtc_clear_a1f(const uint16_t usci_base_addr);
uint8_t dsrtc_triggered_a1(const uint16_t usci_base_addr, uint8_t * val);
uint8_t dsrtc_clear_a2f(const uint16_t usci_base_addr);
uint8_t dsrtc_triggered_a2(const uint16_t usci_base_addr, uint8_t * val);
#endif

#ifdef __cplusplus
}
#endif

#endif
