
#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "atlas430.h"
#include "ui.h"
#include "sig.h"

uart_descriptor bc; // backchannel uart interface

#ifdef TEST_HBMPS
    device_t dev_hbmps;
    bus_desc_i2c_hw_master_t hbmps_bus_desc;
    hbmps_spec_t hbmps_spec;
#endif

#ifdef TEST_DSRTC
    device_t dev_dsrtc;
    dsrtc_priv_t dsrtc_priv;
    bus_desc_i2c_hw_master_t dsrtc_bus_desc;
#endif

#ifdef TEST_CYPRESS_FM24
    device_t dev_fm24;
    bus_desc_i2c_hw_master_t fm24_bus_desc;
#endif

static void uart_bcl_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart_set_eol(&bc);
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart_get_event(&bc) == UART_EV_RX) {
        msg |= SYS_MSG_UART_BCL_RX;
        uart_rst_event(&bc);
    }

    eh_exec(msg);
}

void i2c_init(void)
{

#if defined __MSP430FR2476__
    P1SEL0 |= BIT2 | BIT3;
    P1SEL1 &= ~(BIT2 | BIT3);
#else
    i2c_pin_init();
#endif

#if I2C_USE_DEV > 3
    // enhanced USCI capable microcontroller
    EUSCI_B_I2C_initMasterParam param = {0};

    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = SMCLK_FREQ;
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(I2C_BASE_ADDR, &param);
#elif I2C_USE_DEV < 4
    // USCI capable microcontroller
    USCI_B_I2C_initMasterParam param = {0};

    param.selectClockSource = USCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = SMCLK_FREQ;
    param.dataRate = USCI_B_I2C_SET_DATA_RATE_400KBPS;
    USCI_B_I2C_initMaster(I2C_BASE_ADDR, &param);
#endif

    i2c_irq_init(I2C_BASE_ADDR);

#ifdef TEST_HBMPS
    hbmps_spec.output_min = 0x666;
    hbmps_spec.output_max = 0x399a;
    hbmps_spec.pressure_min = 0;
    hbmps_spec.pressure_max = 206843;

    bus_init_i2c_hw_master(&dev_hbmps, I2C_BASE_ADDR, HSC_SLAVE_ADDR, &hbmps_bus_desc);
#endif

#ifdef TEST_DSRTC
    bus_init_i2c_hw_master(&dev_dsrtc, I2C_BASE_ADDR, DSRTC_I2C_ADDR, &dsrtc_bus_desc);

    dsrtc_priv.ic_type = DSRTC_TYPE_DS3231;
    dev_dsrtc.priv = &dsrtc_priv;
#endif

#ifdef TEST_CYPRESS_FM24
    bus_init_i2c_hw_master(&dev_fm24, I2C_BASE_ADDR, FM24_SLAVE_ADDR, &fm24_bus_desc);
#endif

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

#if defined (__MSP430FR2433__) || defined (__MSP430FR2476__) || defined (__MSP430FR4133__) || defined (__MSP430FR5969__) || defined (__MSP430FR5994__)
    bc.baseAddress = EUSCI_A0_BASE;
#elif defined (__MSP430FR2355__) || defined (__MSP430F5529__)
    bc.baseAddress = EUSCI_A1_BASE;
#elif defined (__CC430F5137__)
    bc.baseAddress = USCI_A0_BASE;
#elif defined (__MSP430F5510__) || defined (__MSP430F5529__)
    bc.baseAddress = USCI_A1_BASE;
#endif
    bc.baudrate = BAUDRATE_57600;

#if defined __MSP430FR6989__
    P3SEL0 |= BIT4 | BIT5;
    P3SEL1 &= ~(BIT4 | BIT5);
#elif defined __MSP430FR2476__
    P1SEL0 |= BIT4 | BIT5;
    P1SEL1 &= ~(BIT4 | BIT5); 
#else
    uart_pin_init(&bc);
#endif
    uart_init(&bc);
#if defined UART_RX_USES_RINGBUF
    uart_set_rx_irq_handler(&bc, uart_rx_ringbuf_handler);
#else
    uart_set_rx_irq_handler(&bc, uart_rx_simple_handler);
#endif

    i2c_init();

#ifdef USE_SIG
    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
    sig5_off;
#endif

    eh_init();
    eh_register(&uart_bcl_rx_irq, SYS_MSG_UART_BCL_RX);
    _BIS_SR(GIE);

    display_version();

    while (1) {
        // sleep
#ifdef USE_SIG
        sig4_off;
#endif
        _BIS_SR(LPM0_bits + GIE);
#ifdef USE_SIG
        sig4_on;
#endif
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
        check_events();
        check_events();
    }

}

