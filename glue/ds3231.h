#ifndef __DS3231_H_
#define __DS3231_H_

//#include "helper.h"

// i2c slave address of the DS3231 chip
#define DS3231_I2C_ADDR             0x68

// timekeeping registers
#define DS3231_TIME_CAL_ADDR        0x00
#define DS3231_ALARM1_ADDR          0x07
#define DS3231_ALARM2_ADDR          0x0B
#define DS3231_CONTROL_ADDR         0x0E
#define DS3231_STATUS_ADDR          0x0F
#define DS3231_AGING_OFFSET_ADDR    0x10
#define DS3231_TEMPERATURE_ADDR     0x11

// control register bits
#define DS3231_CONTROL_A1IE     0x1		/* Alarm 2 Interrupt Enable */
#define DS3231_CONTROL_A2IE     0x2		/* Alarm 2 Interrupt Enable */
#define DS3231_CONTROL_INTCN    0x4		/* Interrupt Control */
#define DS3231_CONTROL_RS1	    0x8		/* square-wave rate select 2 */
#define DS3231_CONTROL_RS2    	0x10	/* square-wave rate select 2 */
#define DS3231_CONTROL_CONV    	0x20	/* Convert Temperature */
#define DS3231_CONTROL_BBSQW    0x40	/* Battery-Backed Square-Wave Enable */
#define DS3231_CONTROL_EOSC	    0x80	/* not Enable Oscillator, 0 equal on */


// status register bits
#define DS3231_STATUS_A1F      0x01		/* Alarm 1 Flag */
#define DS3231_STATUS_A2F      0x02		/* Alarm 2 Flag */
#define DS3231_STATUS_BUSY     0x04		/* device is busy executing TCXO */
#define DS3231_STATUS_EN32KHZ  0x08		/* Enable 32KHz Output  */
#define DS3231_STATUS_OSF      0x80		/* Oscillator Stop Flag */


// status register bits
#define DS3231_A1F      0x1
#define DS3231_A2F      0x2
#define DS3231_OSF      0x80

#ifdef __cplusplus
extern "C" {
#endif

uint8_t DS3231_init(const uint16_t usci_base_addr, const uint8_t creg);
uint8_t DS3231_set(const uint16_t usci_base_addr, struct ts t);
uint8_t DS3231_get(const uint16_t usci_base_addr, struct ts *t);

uint8_t DS3231_set_addr(const uint16_t usci_base_addr, 
                const uint8_t addr, const uint8_t val);
uint8_t DS3231_get_addr(const uint16_t usci_base_addr, 
                const uint8_t addr, uint8_t * val);

// control/status register
uint8_t DS3231_set_creg(const uint16_t usci_base_addr, const uint8_t val);
uint8_t DS3231_set_sreg(const uint16_t usci_base_addr, const uint8_t val);
uint8_t DS3231_get_sreg(const uint16_t usci_base_addr, uint8_t * val);

// aging offset register
uint8_t DS3231_set_aging(const uint16_t usci_base_addr, const int8_t val);
uint8_t DS3231_get_aging(const uint16_t usci_base_addr, int8_t * val);

// temperature register
uint8_t DS3231_get_treg(const uint16_t usci_base_addr, float *temp);

// alarms
uint8_t DS3231_set_a1(const uint16_t usci_base_addr,
                const uint8_t s, const uint8_t mi, const uint8_t h,
                const uint8_t d, const uint8_t * flags);
uint8_t DS3231_get_a1(const uint16_t usci_base_addr, char *buf, const uint8_t len);
uint8_t DS3231_clear_a1f(const uint16_t usci_base_addr);
uint8_t DS3231_triggered_a1(const uint16_t usci_base_addr, uint8_t * val);

uint8_t DS3231_set_a2(const uint16_t usci_base_addr, 
                const uint8_t mi, const uint8_t h, const uint8_t d,
                const uint8_t * flags);
uint8_t DS3231_get_a2(const uint16_t usci_base_addr, char *buf, const uint8_t len);
uint8_t DS3231_clear_a2f(const uint16_t usci_base_addr);
uint8_t DS3231_triggered_a2(const uint16_t usci_base_addr, uint8_t * val);

#ifdef __cplusplus
}
#endif

#endif
