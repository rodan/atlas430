
#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"
#include "sig.h"
#include "uart_mapping.h"

static void uart_bcl_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart_bcl_set_eol();
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart_bcl_get_event() == UART_EV_RX) {
        msg |= SYS_MSG_UART_BCL_RX;
        uart_bcl_rst_event();
    }

    eh_exec(msg);
}

void i2c_init(void)
{
    i2c_pin_init();

#if I2C_USE_DEV > 3
    // enhanced USCI capable microcontroller
    EUSCI_B_I2C_initMasterParam param = {0};

    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = SMCLK_FREQ;// CS_getSMCLK();
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

    uart_bcl_pin_init();
    uart_bcl_init();
#if defined UART0_RX_USES_RINGBUF || defined UART1_RX_USES_RINGBUF || \
    defined UART2_RX_USES_RINGBUF || defined UART3_RX_USES_RINGBUF
    uart_bcl_set_rx_irq_handler(uart_bcl_rx_ringbuf_handler);
#else
    uart_bcl_set_rx_irq_handler(uart_bcl_rx_simple_handler);
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

