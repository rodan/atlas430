
/*
  DS3231 library

  This library implements the following features:

   - read/write of current time, both of the alarms, 
   control/status registers, aging register
   - read of the temperature register, and of any address from the chip.

  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430fr5x
  license:         BSD

  The DS3231 is a low-cost, extremely accurate I2C real-time clock 
  (RTC) with an integrated temperature-compensated crystal oscillator 
  (TCXO) and crystal.
*/

#ifdef CONFIG_DS3231

#ifdef __I2C_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glue.h"
#include "ds3231.h"

#ifdef I2C_USES_BITBANGING
#include "serial_bitbang.h"
#else
#include "i2c.h"
#endif

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

uint8_t DS3231_init(const uint16_t usci_base_addr, const uint8_t ctrl_reg)
{
    return DS3231_set_creg(usci_base_addr, ctrl_reg);
}

uint8_t DS3231_set(const uint16_t usci_base_addr, struct ts t)
{
    uint8_t i, century;

    if (t.year > 2000) {
        century = 0x80;
        t.year_s = t.year - 2000;
    } else {
        century = 0;
        t.year_s = t.year - 1900;
    }

    uint8_t i2c_buff[8] =
        { DS3231_TIME_CAL_ADDR, t.sec, t.min, t.hour, t.wday, t.mday, t.mon,
t.year_s };

    for (i = 1; i <= 7; i++) {
        i2c_buff[i] = dec_to_bcd(i2c_buff[i]);
        if (i == 6) {
            i2c_buff[6] += century;
        }
    }

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = i2c_buff;
    pkg.data_len = 8;
    pkg.options = I2C_WRITE;

#ifdef I2C_USES_BITBANGING
    if (i2cm_transfer(&pkg) != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    return EXIT_SUCCESS;
}

uint8_t DS3231_get(const uint16_t usci_base_addr, struct ts * t)
{
    uint8_t TimeDate[7];        //second,minute,hour,dow,day,month,year
    uint8_t century = 0;
    uint8_t i;
    uint16_t year_full;
    uint8_t i2c_buff[7];
    uint8_t i2c_cmd[1] = { DS3231_TIME_CAL_ADDR };

    i2c_package_t pkg;
    memset(&i2c_buff, 0, 7);
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = i2c_cmd;
    pkg.addr_len = 1;
    pkg.data = i2c_buff;
    pkg.data_len = 7;
    pkg.options = I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ;

#ifdef I2C_USES_BITBANGING
    uint8_t rv = 255;
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    for (i = 0; i <= 6; i++) {
        if (i == 5) {
            TimeDate[5] = bcd_to_dec(i2c_buff[i] & 0x1F);
            century = (i2c_buff[i] & 0x80) >> 7;
        } else
            TimeDate[i] = bcd_to_dec(i2c_buff[i]);
    }

    if (century == 1) {
        year_full = 2000 + TimeDate[6];
    } else {
        year_full = 1900 + TimeDate[6];
    }

    t->sec = TimeDate[0];
    t->min = TimeDate[1];
    t->hour = TimeDate[2];
    t->mday = TimeDate[4];
    t->mon = TimeDate[5];
    t->year = year_full;
    t->wday = TimeDate[3];
    t->year_s = TimeDate[6];
#ifdef CONFIG_UNIXTIME
    t->unixtime = get_unixtime(*t);
#endif

    return EXIT_SUCCESS;
}

uint8_t DS3231_set_addr(const uint16_t usci_base_addr, const uint8_t addr, const uint8_t val)
{
    uint8_t i2c_buff[2] = { addr, val };

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = i2c_buff;
    pkg.data_len = 2;
    pkg.options = I2C_WRITE;

#ifdef I2C_USES_BITBANGING
    if (i2cm_transfer(&pkg) != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    return EXIT_SUCCESS;
}

uint8_t DS3231_get_addr(const uint16_t usci_base_addr, const uint8_t addr, uint8_t * val)
{
    uint8_t i2c_cmd[1]={ addr };

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = i2c_cmd;
    pkg.addr_len = 1;
    pkg.data = val;
    pkg.data_len = 1;
    pkg.options = I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ;

#ifdef I2C_USES_BITBANGING
    uint8_t rv;
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    return EXIT_SUCCESS;
}

// control register

uint8_t DS3231_set_creg(const uint16_t usci_base_addr, const uint8_t val)
{
    return DS3231_set_addr(usci_base_addr, DS3231_CONTROL_ADDR, val);
}

// status register 0Fh/8Fh

/*
bit7 OSF      Oscillator Stop Flag (if 1 then oscillator has stopped and date might be innacurate)
bit3 EN32kHz  Enable 32kHz output (1 if needed)
bit2 BSY      Busy with TCXO functions
bit1 A2F      Alarm 2 Flag - (1 if alarm2 was triggered)
bit0 A1F      Alarm 1 Flag - (1 if alarm1 was triggered)
*/

uint8_t DS3231_set_sreg(const uint16_t usci_base_addr, const uint8_t val)
{
    return DS3231_set_addr(usci_base_addr, DS3231_STATUS_ADDR, val);
}

uint8_t DS3231_get_sreg(const uint16_t usci_base_addr, uint8_t * val)
{
    return DS3231_get_addr(usci_base_addr, DS3231_STATUS_ADDR, val);
}

// aging register
uint8_t DS3231_set_aging(const uint16_t usci_base_addr, const int8_t val)
{
    uint8_t reg;

    if (val >= 0) {
        reg = val;
    } else {
        reg = ~(-val) + 1;      // 2C
    }

    return DS3231_set_addr(usci_base_addr, DS3231_AGING_OFFSET_ADDR, reg);
}

uint8_t DS3231_get_aging(const uint16_t usci_base_addr, int8_t * val)
{
    uint8_t reg;
    uint8_t rv;

    rv = DS3231_get_addr(usci_base_addr, DS3231_AGING_OFFSET_ADDR, &reg);

    if ((reg & 0x80) != 0) {
        *val = reg | ~((1 << 8) - 1);   // if negative get two's complement
    } else {
        *val = reg;
    }

    return rv;
}

// temperature register
uint8_t DS3231_get_treg(const uint16_t usci_base_addr, float *temp)
{
    uint8_t temp_msb, temp_lsb;
    int8_t nint;
    uint8_t i2c_buff[2];
    uint8_t i2c_cmd[1] = { DS3231_TEMPERATURE_ADDR };

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = i2c_cmd;
    pkg.addr_len = 1;
    pkg.data = i2c_buff;
    pkg.data_len = 2;
    pkg.options = I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ;

#ifdef I2C_USES_BITBANGING
    uint8_t rv;
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    temp_msb = i2c_buff[0];
    temp_lsb = i2c_buff[1] >> 6;

    if ((temp_msb & 0x80) != 0) {
        nint = temp_msb | ~((1 << 8) - 1);      // if negative get two's complement
    } else {
        nint = temp_msb;
    }

    *temp = 0.25 * temp_lsb + nint;

    return EXIT_SUCCESS;
}

// alarms

// flags are: A1M1 (seconds), A1M2 (minutes), A1M3 (hour), 
// A1M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0)
uint8_t DS3231_set_a1(const uint16_t usci_base_addr, 
                    const uint8_t s, const uint8_t mi, const uint8_t h,
                    const uint8_t d, const uint8_t * flags)
{
    uint8_t t[4] = { s, mi, h, d };
    uint8_t i;
    uint8_t i2c_buff[5];

    i2c_buff[0] = DS3231_ALARM1_ADDR;

    for (i = 0; i <= 3; i++) {
        if (i == 3) {
            i2c_buff[4] = dec_to_bcd(t[3]) | (flags[3] << 7) | (flags[4] << 6);
        } else {
            i2c_buff[i + 1] = dec_to_bcd(t[i]) | (flags[i] << 7);
        }
    }

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = i2c_buff;
    pkg.data_len = 5;
    pkg.options = I2C_WRITE;

#ifdef I2C_USES_BITBANGING
    if (i2cm_transfer(&pkg) != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    return EXIT_SUCCESS;
}

uint8_t DS3231_get_a1(const uint16_t usci_base_addr, char *buf, const uint8_t len)
{
    uint8_t t[4];               //second,minute,hour,day
    uint8_t f[5];               // flags
    uint8_t i;
    uint8_t i2c_buff[4];
    uint8_t i2c_cmd[1] = { DS3231_ALARM1_ADDR };

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = i2c_cmd;
    pkg.addr_len = 1;
    pkg.data = i2c_buff;
    pkg.data_len = 4;
    pkg.options = I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ;

#ifdef I2C_USES_BITBANGING
    uint8_t rv;
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    for (i = 0; i <= 3; i++) {
        f[i] = (i2c_buff[i] & 0x80) >> 7;
        t[i] = bcd_to_dec(i2c_buff[i] & 0x7F);
    }

    f[4] = (i2c_buff[3] & 0x40) >> 6;
    t[3] = bcd_to_dec(i2c_buff[3] & 0x3F);

    snprintf(buf, len,
             "s%02d m%02d h%02d d%02d fs%d m%d h%d d%d wm%d %d %d %d %d",
             t[0], t[1], t[2], t[3], f[0], f[1], f[2], f[3], f[4], i2c_buff[0],
             i2c_buff[1], i2c_buff[2], i2c_buff[3]);

    return EXIT_SUCCESS;
}

// when the alarm flag is cleared the pulldown on INT is also released
uint8_t DS3231_clear_a1f(const uint16_t usci_base_addr)
{
    uint8_t reg_val;
    uint8_t rv;

    rv = DS3231_get_sreg(usci_base_addr, &reg_val);

    //if (rv == I2C_ACK) {
        rv = DS3231_set_sreg(usci_base_addr, reg_val & ~DS3231_A1F);
    //}

    return rv;
}

uint8_t DS3231_triggered_a1(const uint16_t usci_base_addr, uint8_t * val)
{
    uint8_t reg_val;
    uint8_t rv;

    rv = DS3231_get_sreg(usci_base_addr, &reg_val);

    //if (rv == I2C_ACK) {
        *val = reg_val & DS3231_A1F;
    //}

    return rv;
}

// flags are: A2M2 (minutes), A2M3 (hour), A2M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0) - 
uint8_t DS3231_set_a2(const uint16_t usci_base_addr, 
                const uint8_t mi, const uint8_t h, const uint8_t d,
                const uint8_t * flags)
{
    uint8_t t[3] = { mi, h, d };
    uint8_t i;
    uint8_t i2c_buff[4];

    i2c_buff[0] = DS3231_ALARM2_ADDR;

    for (i = 0; i <= 2; i++) {
        if (i == 2) {
            i2c_buff[3] = dec_to_bcd(t[2]) | (flags[2] << 7) | (flags[3] << 6);
        } else {
            i2c_buff[i + 1] = dec_to_bcd(t[i]) | (flags[i] << 7);
        }
    }

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = NULL;
    pkg.addr_len = 0;
    pkg.data = i2c_buff;
    pkg.data_len = 4;
    pkg.options = I2C_WRITE;

#ifdef I2C_USES_BITBANGING
    if (i2cm_transfer(&pkg) != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    return EXIT_SUCCESS;
}

uint8_t DS3231_get_a2(const uint16_t usci_base_addr, char *buf, const uint8_t len)
{
    uint8_t t[3];               //second,minute,hour,day
    uint8_t f[4];               // flags
    uint8_t i;
    uint8_t i2c_buff[3];
    uint8_t i2c_cmd[1] = { DS3231_ALARM2_ADDR };

    i2c_package_t pkg;
    memset(&pkg, 0, sizeof(i2c_package_t));
    pkg.slave_addr = DS3231_I2C_ADDR;
    pkg.addr = i2c_cmd;
    pkg.addr_len = 1;
    pkg.data = i2c_buff;
    pkg.data_len = 3;
    pkg.options = I2C_READ | I2C_LAST_NAK | I2C_REPEAT_SA_ON_READ;

#ifdef I2C_USES_BITBANGING
    uint8_t rv;
    rv = i2cm_transfer(&pkg);
    if (rv != I2C_ACK) {
        return EXIT_FAILURE;
    }
#else
    i2c_transfer_start(usci_base_addr, &pkg, NULL);
#endif

    for (i = 0; i <= 2; i++) {
        f[i] = (i2c_buff[i] & 0x80) >> 7;
        t[i] = bcd_to_dec(i2c_buff[i] & 0x7F);
    }

    f[3] = (i2c_buff[2] & 0x40) >> 6;
    t[2] = bcd_to_dec(i2c_buff[2] & 0x3F);

    snprintf(buf, len, "m%02d h%02d d%02d fm%d h%d d%d wm%d %d %d %d", t[0],
             t[1], t[2], f[0], f[1], f[2], f[3], i2c_buff[0], i2c_buff[1],
             i2c_buff[2]);

    return EXIT_SUCCESS;
}

// when the alarm flag is cleared the pulldown on INT is also released
uint8_t DS3231_clear_a2f(const uint16_t usci_base_addr)
{
    uint8_t rv;
    uint8_t reg_val;

    rv = DS3231_get_sreg(usci_base_addr, &reg_val);

    //if (rv == I2C_ACK) {
        rv = DS3231_set_sreg(usci_base_addr, reg_val & ~DS3231_A2F);
    //}

    return rv;
}

uint8_t DS3231_triggered_a2(const uint16_t usci_base_addr, uint8_t * val)
{
    uint8_t reg_val;
    uint8_t rv;

    rv = DS3231_get_sreg(usci_base_addr, &reg_val);

    //if (rv == I2C_ACK) {
        *val = reg_val & DS3231_A2F;
    //}

    return rv;
}

#endif // __I2C_CONFIG_H__
#endif // CONFIG_DS3231
