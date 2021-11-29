
/*
  MAXIM DS3234 library for MSP430FR5xxx

  This library implements the following features:

   - read/write of current time, both of the alarms, 
   control/status registers, aging register, sram
   - read of the temperature register, and of any address from the chip.

  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430fr5x
 
  The DS3234 is a low-cost, extremely accurate SPI real-time clock 
  (RTC) with an integrated temperature-compensated crystal oscillator 
  (TCXO) and crystal.
*/

#ifdef CONFIG_DS3234

#include <stdio.h>
#include <stdlib.h>
#include "eusci_b_spi.h"
#include "spi.h"
#include "lib_convert.h"
#include "lib_time.h"
#include "ds3234.h"

/*
// the initialization sequence depends on the uC hardware
// the following is just an example for an MSP430FR5994
void DS3234_init(const spi_descriptor *spid)
{
    spid->cs_high();
    // UCB1MOSI, UCB1MISO, UCB1CLK
    P5SEL0 |= (BIT0 | BIT1 | BIT2);
    P5SEL1 &= ~(BIT0 | BIT1 | BIT2);

    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = 8000000;
    param.desiredSpiClock = 1000000;
    param.msbFirst= EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase= EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(spid->baseAddress, &param);
    EUSCI_B_SPI_enable(spid->baseAddress);
}
*/

uint8_t DS3234_read(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count)
{
    uint8_t txdata = addr;
    spid->cs_low();
    spi_write_frame(spid->baseAddress, &txdata, 1);
    spi_read_frame(spid->baseAddress, buf, count);
    spid->cs_high();

    return 0;
}

uint8_t DS3234_write(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count)
{
    uint8_t txdata = addr | 0x80;
    spid->cs_low();
    spi_write_frame(spid->baseAddress, &txdata, 1);
    spi_write_frame(spid->baseAddress, buf, count);
    spid->cs_high();

    return 0;
}

uint8_t DS3234_read_reg(const spi_descriptor *spid, const uint8_t addr, uint8_t *val)
{
    return DS3234_read(spid, addr, val, 1);
}

uint8_t DS3234_write_reg(const spi_descriptor *spid, const uint8_t addr, const uint8_t val)
{
    uint8_t reg_value = val;
    return DS3234_write(spid, addr, &reg_value, 1);
}

uint8_t DS3234_read_rtc(const spi_descriptor *spid, struct ts *t)
{
    uint8_t rv = 0;
    uint8_t rxbuf[7];
    uint8_t i;

    if ((rv = DS3234_read(spid, DS3234_REG_RTC, (uint8_t *) &rxbuf, 7)) != 0) {
        return rv;
    }

    t->year = 0;
    for (i = 0; i < 7; i++) {
        if (i == 5) {
            if (rxbuf[5] & 0x80) {
                t->year += 100;
            }
            rxbuf[5] = bcd_to_dec(rxbuf[5] & 0x1f);
        } else {
            rxbuf[i] = bcd_to_dec(rxbuf[i]);
        }
    }

    t->sec = rxbuf[0];
    t->min = rxbuf[1];
    t->hour = rxbuf[2];
    t->wday = rxbuf[3];
    t->mday = rxbuf[4];
    t->mon = rxbuf[5];
    t->year_s = rxbuf[6];
    t->year += 1900 + rxbuf[6];
#ifdef CONFIG_UNIXTIME
    t->unixtime = get_unixtime(*t);
#endif

    return rv;
}

uint8_t DS3234_write_rtc(const spi_descriptor *spid, struct ts *t)
{
    uint8_t rv;
    uint8_t i, century = 0;

    if (t->year > 1999) {
        century = 1;
        t->year_s = t->year - 2000;
    } else {
        t->year_s = t->year - 1900;
    }

    uint8_t txbuf[7] = { t->sec, t->min, t->hour, t->wday, t->mday, t->mon, t->year_s };

    for (i = 0; i < 7; i++) {
        txbuf[i] = dec_to_bcd(txbuf[i]);
    }

    if (century) {
        txbuf[5] |= 0x80;
    }

    rv = DS3234_write(spid, DS3234_REG_RTC, (uint8_t *)txbuf, 7);

    return rv;
}

uint8_t DS3234_read_aging(const spi_descriptor *spid, int8_t *value)
{
    uint8_t reg = 0;
    int8_t rv;

    if ((rv = DS3234_read_reg(spid, DS3234_REG_AGING, &reg)) != 0) {
        return rv;
    }

    if (reg & 0x80) {
        // if negative get two's complement
        reg |= ~((1 << 8) - 1);
    }

    return rv;
}

uint8_t DS3234_write_aging(const spi_descriptor *spid, const int8_t value)
{
    uint8_t reg;

    if (value >= 0) {
        reg = value;
    } else {
        // if negative get two's complement
        reg = ~(-value) + 1;
    }

    return DS3234_write_reg(spid, DS3234_REG_AGING, reg);
}

uint8_t DS3234_read_temp(const spi_descriptor *spid, int16_t *value)
{
    uint8_t temp_msb, temp_lsb;
    uint8_t rv = 0;

    if ((rv = DS3234_read_reg(spid, DS3234_REG_TEMP, &temp_msb)) != 0) {
        return rv;
    }

    if ((rv = DS3234_read_reg(spid, DS3234_REG_TEMP+1, &temp_lsb)) != 0) {
        return rv;
    }

    if ((temp_msb & 0x80) != 0) {
        // if negative get two's complement
        temp_msb = temp_msb | ~((1 << 8) - 1);
    }

    *value = (temp_msb * 100) + ((temp_lsb >> 6) * 25);

    return 0;
}

uint8_t DS3234_read_a1(const spi_descriptor *spid, struct ts *t, uint8_t *flags)
{
    uint8_t rv;
    uint8_t rxbuf[4];
    uint8_t c = 3;

    if ((rv = DS3234_read(spid, DS3234_REG_A1, (uint8_t *) &rxbuf, 4)) != 0) {
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

uint8_t DS3234_write_a1(const spi_descriptor *spid, const struct ts *t, const uint8_t flags)
{
    uint8_t txbuf[4] = { dec_to_bcd(t->sec), dec_to_bcd(t->min), dec_to_bcd(t->hour), dec_to_bcd(t->mday) };
    uint8_t c=3;
    uint8_t rv;

    if (flags & DS3234_A2_MHDw_MATCH) {
        txbuf[3] = dec_to_bcd(t->wday) | 0x40;
    }

    while (c--) {
        if (flags & (1 << (c + 1))) {
            txbuf[c] |= 0x80;
        }
    }

    rv = DS3234_write(spid, DS3234_REG_A1, (uint8_t *) &txbuf, 4);

    return rv;
}

uint8_t DS3234_read_a2(const spi_descriptor *spid, struct ts *t, uint8_t *flags)
{
    uint8_t rv;
    uint8_t rxbuf[3];
    uint8_t c = 2;

    if ((rv = DS3234_read(spid, DS3234_REG_A2, (uint8_t *) &rxbuf, 3)) != 0) {
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

uint8_t DS3234_write_a2(const spi_descriptor *spid, const struct ts *t, const uint8_t flags)
{
    uint8_t txbuf[3] = { dec_to_bcd(t->min), dec_to_bcd(t->hour), dec_to_bcd(t->mday) };
    uint8_t c=2;
    uint8_t rv;

    if (flags & DS3234_A2_MHDw_MATCH) {
        txbuf[2] = dec_to_bcd(t->wday) | 0x40;
    }

    while (c--) {
        if (flags & (1 << (c + 1))) {
            txbuf[c] |= 0x80;
        }
    }

    rv = DS3234_write(spid, DS3234_REG_A2, (uint8_t *) &txbuf, 3);
    return rv;
}

uint8_t DS3234_read_sram(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count)
{
    uint8_t rv = 0;

    if ((rv = DS3234_write_reg(spid, DS3234_REG_SRAM_ADDR, addr)) != 0) {
        return rv;
    }
   
    rv = DS3234_read(spid, DS3234_REG_SRAM_DATA, buf, count);
    return rv;
}

uint8_t DS3234_write_sram(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count)
{
    uint8_t rv = 0;

    if ((rv = DS3234_write_reg(spid, DS3234_REG_SRAM_ADDR, addr)) != 0) {
        return rv;
    }
   
    rv = DS3234_write(spid, DS3234_REG_SRAM_DATA, buf, count);
    return rv;
}

#endif

