
/*
    program that tests the functionality of the EUSCI A0 UART 

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "sig.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"

volatile uint8_t port5_last_event;
spi_descriptor spid_ds3234;

void main_init(void)
{
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);

    // 5.3 is the ~INT pin
    P5OUT = 0;
    P5DIR = 0x97;

    // activate pullup
    P5OUT = 0x68;
    P5DIR = 0x97;

    // IRQ triggers on the falling edge
    P5IES = 0x08;

    // Reset IRQ flags
    P5IFG &= ~BIT3;
    // Enable button interrupt
    P5IE |= BIT3;
}

void ds3234_cs_low(void)
{
    P3OUT &= ~BIT5;
}

void ds3234_cs_high(void)
{
    P3OUT |= BIT5;
}

void ds3234_init(void)
{
    ds3234_cs_high();
    spid_ds3234.baseAddress = SPI_BASE_ADDR;
    spid_ds3234.cs_low = ds3234_cs_low;
    spid_ds3234.cs_high = ds3234_cs_high;

    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = 8000000;
    param.desiredSpiClock = 1000000;
    param.msbFirst= EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase= EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(spid_ds3234.baseAddress, &param);
    EUSCI_B_SPI_enable(spid_ds3234.baseAddress);
}

static void uart_bcl_rx_handler(uint32_t msg)
{
    parse_user_input();
    uart_bcl_set_eol();
}

static void ds3234_irq_handler(uint32_t msg)
{
    if (P5IN & BIT3) {
        uart_bcl_print("alarm released\r\n");
    } else {
        uart_bcl_print("alarm asserted\r\n");
    }
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart_bcl_get_event() == UART_EV_RX) {
        msg |= SYS_MSG_UART_BCL_RX;
        uart_bcl_rst_event();
    }

    // P5.3 interrupt
    if (port5_last_event) {
        if (port5_last_event & BIT3) {
            msg |= SYS_MSG_P53_INT;
            port5_last_event ^= BIT3;
        }
    }

    eh_exec(msg);
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
    main_init();
#ifdef USE_SIG
    sig0_on;
#endif

    clock_pin_init();
    clock_init();

#if defined __MSP430FR6989__
    P3SEL0 |= BIT4 | BIT5;
    P3SEL1 &= ~(BIT4 | BIT5);
#elif defined __MSP430FR2476__
    P1SEL0 |= BIT4 | BIT5;
    P1SEL1 &= ~(BIT4 | BIT5); 
#else
    uart_bcl_pin_init();
#endif
    uart_bcl_init();
#if defined UART0_RX_USES_RINGBUF || defined UART1_RX_USES_RINGBUF || \
    defined UART2_RX_USES_RINGBUF || defined UART3_RX_USES_RINGBUF
    uart_bcl_set_rx_irq_handler(uart_bcl_rx_ringbuf_handler);
#else
    uart_bcl_set_rx_irq_handler(uart_bcl_rx_simple_handler);
#endif

    spi_pin_init();

#ifdef CONFIG_DS3234
    ds3234_init();
#endif

#ifdef USE_SIG
    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
#endif

    eh_init();
    eh_register(&uart_bcl_rx_handler, SYS_MSG_UART_BCL_RX);
    eh_register(&ds3234_irq_handler, SYS_MSG_P53_INT);
    _BIS_SR(GIE);

    display_version();

    while (1) {
        // sleep
#ifdef USE_SIG
        sig4_off;
#endif
        _BIS_SR(LPM3_bits + GIE);
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

// Port 5 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT5_VECTOR
__interrupt void port5_isr_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT5_VECTOR))) port5_isr_handler(void)
#else
#error Compiler not supported!
#endif
{
    switch (P5IV) {
    case P5IV__P5IFG3:
        port5_last_event |= BIT3;
        // listen for opposite edge
        //P5IES ^= BIT5;
        _BIC_SR_IRQ(LPM3_bits);
        break;
    }
    P5IFG = 0;
}

