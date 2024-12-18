
/*
  Analog Devices AD7789 library for MSP430FR5xxx

  This library implements the following features:


  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430fr5x
 
  The AD7789 is a low power, low noise, analog front end for low frequency
  measurement applications. It contains a low noise 24bit sigma-delta ADC 
  with one differential input.
*/

#include "warning.h"

#ifdef CONFIG_AD7789

#include <math.h>
#include "eusci_b_spi.h"
#include "atlas430.h"
#include "spi.h"
#include "ad7789.h"


uint8_t AD7789_get_status(const spi_descriptor *spid, uint8_t *status)
{
    uint16_t tmout = 10;
    uint8_t txdata = (AD7789_STATUS << 4) | AD7789_READ;
    *status = txdata;

    spid->cs_low();

    while (spid->dev_is_busy() && tmout) {
        tmout--;
    }

    if (tmout==0) {
        spid->cs_high();
        return 0xee; // timeout
    } else {
        spid->spi_init();
        spi_write_frame(spid->baseAddress, &txdata, 1);
        spi_read_frame(spid->baseAddress, status, 1);
    }

    spid->cs_high();
    spid->spi_deinit();

    return EXIT_SUCCESS;
}

uint8_t AD7789_rst(const spi_descriptor *spid)
{
    uint8_t txdata[4] = {0xff, 0xff, 0xff, 0xff};

    spid->spi_init();
    spid->cs_low();
    spi_write_frame(spid->baseAddress, &txdata[0], 4);
    spid->cs_high();
    spid->spi_deinit();
    return EXIT_SUCCESS;
}

uint8_t AD7789_get_conv(const spi_descriptor *spid, uint32_t *conv)
{
    uint16_t tmout = 65535;
    uint8_t  txdata = (AD7789_DATA << 4) | AD7789_READ;
    uint8_t data[3];

    spid->cs_low();

    while (spid->dev_is_busy() && tmout) {
        tmout--;
    }

    if (tmout==0) {
        spid->cs_high();
        return 0xee; // timeout
    } else {
        spid->spi_init();
        spi_write_frame(spid->baseAddress, &txdata, 1);
        spi_read_frame(spid->baseAddress, data, 3);
    }

    spid->cs_high();
    spid->spi_deinit();
   
    *conv = (uint32_t) ((uint32_t) data[0] << 16) + ((uint32_t) data[1] << 8) + data[2];
    return EXIT_SUCCESS;
}

#endif

