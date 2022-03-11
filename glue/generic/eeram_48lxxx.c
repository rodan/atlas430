
/*
  Microchip 48L512/48LM01 serial EERAM driver

  This library implements the following features:

   - read/write of the SRAM
   - read/write of the status register

  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

*/

#include "warning.h"

#ifdef CONFIG_EERAM_48LXXX

#include <stdio.h>
#include <stdlib.h>
#include "eusci_b_spi.h"
#include "spi.h"
#include "lib_convert.h"
#include "lib_time.h"
#include "eeram_48lxxx.h"

/*
// the initialization sequence depends on the uC hardware
// the following is just an example for an MSP430FR5994
void EERAM_48L_init(const spi_descriptor *spid)
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

#if 0
uint8_t EERAM_48L_read(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count)
{
    uint8_t txdata = addr;
    spid->cs_low();
    spi_write_frame(spid->baseAddress, &txdata, 1);
    spi_read_frame(spid->baseAddress, buf, count);
    spid->cs_high();

    return 0;
}

uint8_t EERAM_48L_write(const spi_descriptor *spid, const uint8_t addr, uint8_t *buf, const uint8_t count)
{
    uint8_t txdata = addr | 0x80;
    spid->cs_low();
    spi_write_frame(spid->baseAddress, &txdata, 1);
    spi_write_frame(spid->baseAddress, buf, count);
    spid->cs_high();

    return 0;
}
#endif

uint8_t EERAM_48L_read_streg(const spi_descriptor *spid, uint8_t *val)
{
    uint8_t opcode = EERAM_48L512_RDSR;
    uint8_t rv;

    spid->cs_low();
    spi_write_frame(spid->baseAddress, &opcode, 1);
    rv = spi_read_frame(spid->baseAddress, val, 1);
    spid->cs_high();

    return rv;
}

uint8_t EERAM_48L_write_streg(const spi_descriptor *spid, const uint8_t val)
{
    uint8_t opcode;
    uint8_t stval = val;
    uint8_t rv;

    spid->cs_low();
    opcode = EERAM_48L512_WREN;
    spi_write_frame(spid->baseAddress, &opcode, 1);
    spid->cs_high();

    __no_operation();

    spid->cs_low();
    opcode = EERAM_48L512_WRSR;
    spi_write_frame(spid->baseAddress, &opcode, 1);
    rv = spi_write_frame(spid->baseAddress, &stval, 1);
    spid->cs_high();

    return rv;
}

#endif

