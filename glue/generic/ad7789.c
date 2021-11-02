
/*
  Analog Devices AD7789 library for MSP430FR5xxx

  This library implements the following features:


  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430fr5x
 
  The AD7789 is a low power, low noise, analog front end for low frequency
  measurement applications. It contains a low noise 24bit sigma-delta ADC 
  with one differential input.
*/

#ifdef CONFIG_AD7789

#include <math.h>
#include "eusci_b_spi.h"
#include "config.h"
#include "glue.h"
#include "ad7789.h"

#ifndef AD7789_CS_HIGH
#error "Invalid AD7789_CS_HIGH in config.h"
#endif

#ifndef AD7789_CS_LOW
#error "Invalid AD7789_CS_LOW in config.h"
#endif

void AD7789_init(const uint16_t baseAddress)
{
    EUSCI_B_SPI_initMasterParam param = {0};
/*
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_ACLK;
    param.clockSourceFrequency = 32768;
    //param.desiredSpiClock = 8192;
    param.desiredSpiClock = 4096;
*/    
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = 8000000;
    param.desiredSpiClock = 10000;

    param.msbFirst= EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    //param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    //param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(baseAddress, &param);
    EUSCI_B_SPI_enable(baseAddress);
    AD7789_rst(baseAddress);
    AD7789_deinit_spi();
}

void AD7789_init_spi(void)
{
    //AD7789_CS_HIGH;
    // UCB1MOSI, UCB1MISO, UCB1CLK
    P5SEL0 |= (BIT0 | BIT1 | BIT2);
    P5SEL1 &= ~(BIT0 | BIT1 | BIT2);
}

void AD7789_deinit_spi(void)
{
    //AD7789_CS_HIGH;
    // UCB1MOSI, UCB1MISO, UCB1CLK
    P5SEL0 &= ~(BIT0 | BIT1 | BIT2);
    P5SEL1 &= ~(BIT0 | BIT1 | BIT2);

    P5DIR |= BIT0 | BIT2;
    P5OUT |= BIT0 | BIT2;
    // transform MISO into input GPIO
    P5DIR &= ~BIT1;
    // enable pull-up
    P5OUT |= BIT1;
    P5REN |= BIT1;
}

uint8_t AD7789_get_status(const uint16_t baseAddress, uint8_t *status)
{
    uint16_t tmout = 10;
    uint8_t txdata = (AD7789_STATUS << 4) | AD7789_READ;
    *status = txdata;

    //AD7789_deinit_spi();
    AD7789_CS_LOW;

    while ((P5IN & BIT1) && tmout) {
    //while (tmout) {
        //if (!(P5IN & BIT1)) {
        //    break;
        //}
        tmout--;
    }

    if (tmout==0) {
        //AD7789_CS_HIGH;
        return 0xee; // timeout
    } else {
        AD7789_init_spi();
        spi_send_frame(baseAddress, &txdata, 1);
        //AD7789_CS_HIGH;
        //AD7789_CS_LOW;
        spi_read_frame(baseAddress, status, 1);
    }

    //AD7789_CS_HIGH;
    AD7789_CS_HIGH;
    AD7789_deinit_spi();

    return EXIT_SUCCESS;
}

uint8_t AD7789_rst(const uint16_t baseAddress)
{
    uint8_t txdata[4] = {0xff, 0xff, 0xff, 0xff};

    AD7789_init_spi();
    AD7789_CS_LOW;
    spi_send_frame(baseAddress, &txdata[0], 4);
    AD7789_CS_HIGH;
    AD7789_deinit_spi();
    return EXIT_SUCCESS;
}

uint8_t AD7789_get_conv(const uint16_t baseAddress, uint32_t *conv)
{
    uint16_t tmout = 65535;
    uint8_t  txdata = (AD7789_DATA << 4) | AD7789_READ;
    uint8_t data[3];
    //uint8_t txdata = 0x9c;
    //*status = txdata;

    AD7789_CS_LOW;

    while ((P5IN & BIT1) && tmout) {
        tmout--;
    }

    if (tmout==0) {
        AD7789_CS_HIGH;
        return 0xee; // timeout
    } else {
        AD7789_init_spi();
        spi_send_frame(baseAddress, &txdata, 1);
        spi_read_frame(baseAddress, data, 3);
    }

    AD7789_CS_HIGH;
    AD7789_deinit_spi();
   
    *conv = (uint32_t) ((uint32_t) data[0] << 16) + ((uint32_t) data[1] << 8) + data[2];
    return EXIT_SUCCESS;
}

#endif

