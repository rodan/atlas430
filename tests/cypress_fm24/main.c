
#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "timer_a0.h"
#include "ui.h"
#include "sig.h"
//#include "i2c_config.h"

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

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
    sig0_on;

#ifdef HARDWARE_I2C
    i2c_ucb2_pin_init();
#endif

    clock_pin_init();
    clock_init();

    timer_a0_init();

    uart0_pin_init();
    uart0_init();

#ifdef UART0_RX_USES_RINGBUF
    uart0_set_rx_irq_handler(uart0_rx_ringbuf_handler);
#else
    uart0_set_rx_irq_handler(uart0_rx_simple_handler);
#endif

#ifdef HARDWARE_I2C 
    EUSCI_B_I2C_initMasterParam param = {0};

    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_BASE_ADDR, &param);

    #ifdef I2C_USES_IRQ
        i2c_irq_init(EUSCI_BASE_ADDR);
    #endif
#endif

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;

    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);
    _BIS_SR(GIE);

    display_version();

    while (1) {
        // sleep
        //sig4_off;
        _BIS_SR(LPM3_bits + GIE);
        //sig4_on;

        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
        check_events();
        check_events();
    }

}

