
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

#if defined __MSP430FR5994__
volatile uint8_t port5_last_event;
#endif

uart_descriptor bc; // backchannel uart interface

static void uart_bcl_rx_handler(uint32_t msg)
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

#if defined __MSP430FR5994__
    // P5.3 interrupt
    if (port5_last_event) {
        if (port5_last_event & BIT3) {
            msg |= SYS_MSG_P53_INT;
            port5_last_event ^= BIT3;
        }
    }
#endif

    eh_exec(msg);
}


#ifdef CONFIG_DS3234
spi_descriptor spid_ds3234;

void ds3234_cs_low(void)
{

#if defined __MSP430FR2355__
    P4OUT &= ~BIT4;
#elif defined __MSP430FR2433__
    P3OUT &= ~BIT1;
#elif defined __MSP430FR2476__
    P4OUT &= ~BIT2;
#elif defined __MSP430FR4133__
    P1OUT &= ~BIT3;
#elif defined __CC430F5137__
    P3OUT &= ~BIT7;
#elif defined __MSP430F5510__
    P2OUT &= ~BIT0;
#elif defined __MSP430F5529__
    P2OUT &= ~BIT6;
#elif defined __MSP430FR5969__
    P1OUT &= ~BIT5;
#elif defined __MSP430FR5994__
    P3OUT &= ~BIT5;
#elif defined __MSP430FR6989__
    P2OUT &= ~BIT5;
#endif
}

void ds3234_cs_high(void)
{
#if defined __MSP430FR2355__
    P4OUT |= BIT4;
#elif defined __MSP430FR2433__
    P3OUT |= BIT1;
#elif defined __MSP430FR2476__
    P4OUT |= BIT2;
#elif defined __MSP430FR4133__
    P1OUT |= BIT3;
#elif defined __CC430F5137__
    P3OUT |= BIT7;
#elif defined __MSP430F5510__
    P2OUT |= BIT0;
#elif defined __MSP430F5529__
    P2OUT |= BIT6;
#elif defined __MSP430FR5969__
    P1OUT |= BIT5;
#elif defined __MSP430FR5994__
    P3OUT |= BIT5;
#elif defined __MSP430FR6989__
    P2OUT |= BIT5;
#endif
}

void ds3234_init(void)
{
    ds3234_cs_high();
    spid_ds3234.baseAddress = SPI_BASE_ADDR;
    spid_ds3234.cs_low = ds3234_cs_low;
    spid_ds3234.cs_high = ds3234_cs_high;

#if defined __MSP430_HAS_EUSCI_Bx__
    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = SMCLK_FREQ;
    param.desiredSpiClock = 1000000;
    param.msbFirst= EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase= EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(spid_ds3234.baseAddress, &param);
    EUSCI_B_SPI_enable(spid_ds3234.baseAddress);

#elif defined __MSP430_HAS_USCI_Bx__
    USCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = USCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = SMCLK_FREQ;
    param.desiredSpiClock = 1000000;
    param.msbFirst= USCI_B_SPI_MSB_FIRST;
    param.clockPhase= USCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    USCI_B_SPI_initMaster(spid_ds3234.baseAddress, &param);
    USCI_B_SPI_enable(spid_ds3234.baseAddress);
#endif

}

#if defined __MSP430FR5994__
static void ds3234_irq_handler(uint32_t msg)
{
    if (P5IN & BIT3) {
        uart_print(&bc, "alarm released\r\n");
    } else {
        uart_print(&bc, "alarm asserted\r\n");
    }
}
#endif

#endif



int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;

    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
#if defined __MSP430FR5994__
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
#endif

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
#if defined __MSP430FR5994__
    eh_register(&ds3234_irq_handler, SYS_MSG_P53_INT);
#endif
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


#if defined __MSP430FR5994__
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
#endif
