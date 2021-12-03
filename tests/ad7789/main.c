
/*
    program that tests the functionality of the AD7789 IC
    to be run on MSP430FR5994

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "sig.h"
#include "ui.h"

spi_descriptor spid_ad7789;

void ad7789_cs_low(void)
{
    P5OUT &= ~BIT3;
}

void ad7789_cs_high(void)
{
    P5OUT |= BIT3;
}

void ad7789_spi_init(void)
{
    //AD7789_CS_HIGH;
    // UCB1MOSI, UCB1MISO, UCB1CLK
    P5SEL0 |= (BIT0 | BIT1 | BIT2);
    P5SEL1 &= ~(BIT0 | BIT1 | BIT2);
}

void ad7789_spi_deinit(void)
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

uint8_t ad7789_dev_is_busy(void)
{
    if (P5IN & BIT1) {
        return 1;
    }

    return 0;
}

static void uart0_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart0_set_eol();
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart0_get_event() == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_rst_event();
    }

    eh_exec(msg);
}

void AD7789_init(const spi_descriptor *spid)
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
    EUSCI_B_SPI_initMaster(spid->baseAddress, &param);
    EUSCI_B_SPI_enable(spid->baseAddress);
    AD7789_rst(spid);
    spid->spi_deinit();
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
#ifdef USE_SIG
    sig0_on;
#endif

    clock_pin_init();
    clock_init();

    uart0_pin_init();
    uart0_init();

#ifdef UART0_RX_USES_RINGBUF
    uart0_set_rx_irq_handler(uart0_rx_ringbuf_handler);
#else
    uart0_set_rx_irq_handler(uart0_rx_simple_handler);
#endif

    spid_ad7789.baseAddress = EUSCI_SPI_BASE_ADDR;
    spid_ad7789.cs_low = ad7789_cs_low;
    spid_ad7789.cs_high = ad7789_cs_high;
    spid_ad7789.spi_init = ad7789_spi_init;
    spid_ad7789.spi_deinit = ad7789_spi_deinit;
    spid_ad7789.dev_is_busy = ad7789_dev_is_busy;

    AD7789_init(&spid_ad7789);

#ifdef USE_SIG
    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
#endif

    eh_init();
    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);
    _BIS_SR(GIE);

    display_version();

    while (1) {
        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
    }
}
