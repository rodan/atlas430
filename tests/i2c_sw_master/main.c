
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
    bus_desc_i2c_sw_master_t hbmps_bus_desc;
    hbmps_spec_t hbmps_spec;
#endif

#ifdef TEST_DSRTC
    device_t dev_dsrtc;
    dsrtc_priv_t dsrtc_priv;
    bus_desc_i2c_sw_master_t dsrtc_bus_desc;
#endif

#ifdef TEST_CYPRESS_FM24
    device_t dev_fm24;
    fm24_spec_t fm24_spec;
    bus_desc_i2c_sw_master_t fm24_bus_desc;
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

#if !defined __MSP430FR5994__
    #error "this test is compatible only with MSP430FR5994"
#endif

#ifdef TEST_HBMPS
    hbmps_spec.output_min = 0x666;
    hbmps_spec.output_max = 0x399a;
    hbmps_spec.pressure_min = 0;
    hbmps_spec.pressure_max = 206843;
    dev_hbmps.spec = &hbmps_spec;

    hbmps_bus_desc.port_dir = HWREGADDR8(P7DIR);
    hbmps_bus_desc.port_out = HWREGADDR8(P7OUT);
    hbmps_bus_desc.port_in = HWREGADDR8(P7IN);
    hbmps_bus_desc.pin_scl = BIT1;
    hbmps_bus_desc.pin_sda = BIT0;

    bus_init_i2c_sw_master(&dev_hbmps, HSC_SLAVE_ADDR, &hbmps_bus_desc);
#endif

#ifdef TEST_DSRTC
    dsrtc_bus_desc.port_dir = HWREGADDR8(P7DIR);
    dsrtc_bus_desc.port_out = HWREGADDR8(P7OUT);
    dsrtc_bus_desc.port_in = HWREGADDR8(P7IN);
    dsrtc_bus_desc.pin_scl = BIT1;
    dsrtc_bus_desc.pin_sda = BIT0;

    bus_init_i2c_sw_master(&dev_dsrtc, DSRTC_SLAVE_ADDR, &dsrtc_bus_desc);

    dsrtc_priv.ic_type = DSRTC_TYPE_DS3231;
    dev_dsrtc.priv = &dsrtc_priv;
#endif

#ifdef TEST_CYPRESS_FM24
    fm24_bus_desc.port_dir = HWREGADDR8(P7DIR);
    fm24_bus_desc.port_out = HWREGADDR8(P7OUT);
    fm24_bus_desc.port_in = HWREGADDR8(P7IN);
    fm24_bus_desc.pin_scl = BIT1;
    fm24_bus_desc.pin_sda = BIT0;

    fm24_spec.mem_sz = 0x1FFFl;
    dev_fm24.spec = &fm24_spec;
    bus_init_i2c_sw_master(&dev_fm24, FM24_SLAVE_ADDR, &fm24_bus_desc);
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

