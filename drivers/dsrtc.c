
/*
  DS3231/DS3234 device driver

  capabilities:
   - read/write of current time, both of the alarms, 
   control/status registers, aging register
   - read of the temperature register, and of any address from the chip.
   - read/write SRAM area

  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430
  license:         BSD

  The DS3231/3234 is a low-cost, extremely accurate I2C/SPI real-time clock
  with an integrated temperature-compensated crystal oscillator
  (TCXO) and crystal.
*/

#include "warning.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atlas430.h"
#include "bus.h"
#include "dsrtc.h"

uint16_t dsrtc_read(device_t *dev, const uint8_t offset, uint8_t *buf, const uint8_t nbytes)
{
    uint16_t rv;
    uint8_t cmd[1];

    cmd[0] = offset;
    memset(buf, 0, nbytes);
    rv = bus_read(dev, buf, nbytes, cmd, 1);
    return rv;
}

uint16_t dsrtc_write(device_t *dev, const uint8_t offset, uint8_t *buf, const uint8_t nbytes)
{
    uint16_t rv;

    rv = bus_write(dev, buf, nbytes, NULL, 0);
    return rv;
}

uint16_t dsrtc_read_reg(device_t *dev, const uint8_t addr, uint8_t *val)
{
    uint16_t rv;
    uint8_t buf[1] = { addr };

    rv = bus_read(dev, val, 1, buf, 1);
    return rv;
}

uint16_t dsrtc_write_reg(device_t *dev, const uint8_t addr, const uint8_t val)
{
    uint16_t rv;
    uint8_t buf[2] = { addr, val };

    rv = bus_write(dev, buf, 2, NULL, 0);
    return rv;
}

uint16_t dsrtc_read_rtc(device_t *dev, struct ts *t)
{
    uint16_t rv = BUS_OK;
    uint8_t buf[7];
    uint8_t i;

    if ((rv = dsrtc_read(dev, DSRTC_REG_RTC, buf, 7)) != 0) {
        return rv;
    }

    t->year = 0;
    if (buf[5] & 0x80) {
        t->year += 100;
        buf[5] &= ~0x80;
    }

    for (i = 0; i < 7; i++) {
        buf[i] = bcd_to_dec(buf[i]);
    }

    t->sec = buf[0];
    t->min = buf[1];
    t->hour = buf[2];
    t->wday = buf[3];
    t->mday = buf[4];
    t->mon = buf[5];
    t->year_s = buf[6];
    t->year += 1900 + buf[6];
#ifdef CONFIG_UNIXTIME
    t->unixtime = get_unixtime(*t);
#endif

    return rv;
}

uint16_t dsrtc_write_rtc(device_t *dev, struct ts *t)
{
    uint16_t rv;
    uint8_t i, century = 0;
    dsrtc_priv_t *p;

    if (!dev->priv)
        return BUS_ERR_ARG;

    if (t->year > 1999) {
        century = 1;
        t->year_s = t->year - 2000;
    } else {
        t->year_s = t->year - 1900;
    }

    uint8_t buf[8] = { DSRTC_REG_RTC, t->sec, t->min, t->hour, t->wday, t->mday, t->mon, t->year_s };

    for (i = 1; i < 8; i++) {
        buf[i] = dec_to_bcd(buf[i]);
    }
    if (century) {
        buf[6] |= 0x80;
    }

    p = (dsrtc_priv_t *) dev->priv;
    switch (p->ic_type) {
        case DSRTC_TYPE_DS3231:
            rv = dsrtc_write(dev, DSRTC_REG_RTC, &buf[0], 8);
            break;
        case DSRTC_TYPE_DS3234:
            rv = dsrtc_write(dev, DSRTC_REG_RTC, &buf[1], 7);
            break;
        default:
            return BUS_ERR_ARG;
            break;
    }

    return rv;
}

uint16_t dsrtc_read_a1(device_t *dev, struct ts *t, uint8_t *flags)
{
    uint16_t rv;
    uint8_t rxbuf[4];
    uint8_t c = 3;

    rv = dsrtc_read(dev, DSRTC_REG_A1, (uint8_t *) &rxbuf, 4);
    if (rv != BUS_OK) {
        return rv;
    }

    *flags = 0;
    while (c--) {
        if (rxbuf[c] & 0x80) {
            *flags |= 1 << (c + 1);
        }
    }

    t->sec = bcd_to_dec(rxbuf[0] & 0x7f);
    t->min = bcd_to_dec(rxbuf[1] & 0x7f);
    t->hour = bcd_to_dec(rxbuf[2] & 0x7f);

    if (rxbuf[3] & 0x10) {
        t->wday = bcd_to_dec(rxbuf[3] & 0x7f);
        *flags |= 0x10;
    } else {
        t->mday = bcd_to_dec(rxbuf[3] & 0x7f);
    }

    return rv;
}

uint16_t dsrtc_write_a1(device_t *dev, const struct ts *t, const uint8_t flags)
{
    uint8_t txbuf[4] = { dec_to_bcd(t->sec), dec_to_bcd(t->min), dec_to_bcd(t->hour), dec_to_bcd(t->mday) };
    uint8_t c=3;
    uint16_t rv;

    if (flags & DSRTC_A2_MHDw_MATCH) {
        txbuf[3] = dec_to_bcd(t->wday) | 0x40;
    }

    while (c--) {
        if (flags & (1 << (c + 1))) {
            txbuf[c] |= 0x80;
        }
    }

    rv = dsrtc_write(dev, DSRTC_REG_A1, (uint8_t *) &txbuf, 4);
    return rv;
}

uint16_t dsrtc_read_a2(device_t *dev, struct ts *t, uint8_t *flags)
{
    uint16_t rv;
    uint8_t rxbuf[3];
    uint8_t c = 2;

    rv = dsrtc_read(dev, DSRTC_REG_A2, (uint8_t *) &rxbuf, 3);
    if (rv != BUS_OK) {
        return rv;
    }

    *flags = 0;
    while (c--) {
        if (rxbuf[c] & 0x80) {
            *flags |= 1 << (c + 1);
        }
    }

    t->min = bcd_to_dec(rxbuf[0] & 0x7f);
    t->hour = bcd_to_dec(rxbuf[1] & 0x7f);

    if (rxbuf[2] & 0x10) {
        t->wday = bcd_to_dec(rxbuf[2] & 0x7f);
        *flags |= 0x08;
    } else {
        t->mday = bcd_to_dec(rxbuf[2] & 0x7f);
    }

    return rv;
}

uint16_t dsrtc_write_a2(device_t *dev, const struct ts *t, const uint8_t flags)
{
    uint8_t txbuf[3] = { dec_to_bcd(t->min), dec_to_bcd(t->hour), dec_to_bcd(t->mday) };
    uint8_t c=2;
    uint16_t rv;

    if (flags & DSRTC_A2_MHDw_MATCH) {
        txbuf[2] = dec_to_bcd(t->wday) | 0x40;
    }

    while (c--) {
        if (flags & (1 << (c + 1))) {
            txbuf[c] |= 0x80;
        }
    }

    rv = dsrtc_write(dev, DSRTC_REG_A2, (uint8_t *) &txbuf, 3);
    return rv;
}

uint16_t dsrtc_read_aging(device_t *dev, int8_t *value)
{
    uint8_t reg = 0;
    uint16_t rv;

    rv = dsrtc_read_reg(dev, DSRTC_REG_AGING, &reg);
    if (rv != BUS_OK) {
        return rv;
    }

    if (reg & 0x80) {
        *value = reg | ~((1 << 8) - 1);   // if negative get two's complement
    } else {
        *value = reg;
    }

    return rv;
}

uint16_t dsrtc_write_aging(device_t *dev, const int8_t value)
{
    uint8_t reg;
    uint16_t rv;

    if (value >= 0) {
        reg = value;
    } else {
        // if negative get two's complement
        reg = ~(-value) + 1;
    }

    rv = dsrtc_write_reg(dev, DSRTC_REG_AGING, reg);
    return rv;
}

uint16_t dsrtc_read_temp(device_t *dev, int16_t *value)
{
    uint8_t temp_msb, temp_lsb;
    uint16_t rv;
    uint8_t buf[2];

    rv = dsrtc_read(dev, DSRTC_REG_TEMP, buf, 2);
    if (rv != BUS_OK) {
        return rv;
    }

    temp_msb = buf[0];
    temp_lsb = buf[1] >> 6;

    if ((temp_msb & 0x80) != 0) {
        // if negative get two's complement
        temp_msb = temp_msb | ~((1 << 8) - 1);
    }

    *value = (temp_msb * 100) + (temp_lsb * 25);
    return rv;
}

uint16_t dsrtc_read_sram(device_t *dev, const uint8_t offset, uint8_t *buf, const uint8_t nbytes)
{
    uint16_t rv;

    rv = dsrtc_write_reg(dev, DSRTC_REG_SRAM_ADDR, offset);
    if (rv != BUS_OK) {
        return rv;
    }
   
    rv = dsrtc_read(dev, DSRTC_REG_SRAM_DATA, buf, nbytes);
    return rv;
}

uint16_t dsrtc_write_sram(device_t *dev, const uint8_t offset, uint8_t *buf, const uint8_t nbytes)
{
    uint16_t rv = 0;

    rv = dsrtc_write_reg(dev, DSRTC_REG_SRAM_ADDR, offset);
    if (rv != BUS_OK) {
        return rv;
    }
   
    rv = dsrtc_write(dev, DSRTC_REG_SRAM_DATA, buf, nbytes);
    return rv;
}
