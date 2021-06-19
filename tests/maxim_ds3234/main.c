
/*
    program that tests the functionality of the EUSCI A0 UART 

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"

volatile uint8_t port5_last_event;
spi_descriptor spid_ds3234;

void main_init(void)
{
    // set all ports as output, low level

    P1OUT = 0;
    P1DIR = 0xFF;

    P2OUT = 0;
    P2DIR = 0xFF;

    P3OUT = 0;
    P3DIR = 0xFF;

    P4OUT = 0;
    P4DIR = 0xFF;

    // 5.3 is the ~INT pin
    P5OUT = 0;
    P5DIR = 0x97;

    // activate pullup
    P5OUT = 0x68;
    P5DIR = 0x97;

    // IRQ triggers on the falling edge
    P5IES = 0x08;

    // Reset IRQ flags
    P5IFG &= ~(BIT3);
    // Enable button interrupt
    P5IE |= BIT3;

    P6OUT = 0;
    P6DIR = 0xFF;

    P7OUT = 0;
    P7DIR = 0xFF;

    P8DIR = 0xFF;
    P8OUT = 0;

    PJOUT = 0;
    PJDIR = 0xFFFF;

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
    EUSCI_B_SPI_initMaster(spid_ds3234.baseAddress, &param);
    EUSCI_B_SPI_enable(spid_ds3234.baseAddress);
}

static void uart0_rx_handler(uint32_t msg)
{
    parse_user_input();
    uart0_set_eol();
}

static void ds3234_irq_handler(uint32_t msg)
{
    if (P5IN & BIT3) {
        uart0_print("alarm released\r\n");
    } else {
        uart0_print("alarm asserted\r\n");
    }
}

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart0_get_event() == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_rst_event();
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
    main_init();
    sig0_on;

    clock_port_init();
    clock_init();

    // output SMCLK on P3.4
    P3OUT &= ~BIT4;
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;

    uart0_port_init();
    uart0_init();

#ifdef UART0_RX_USES_RINGBUF
    uart0_set_rx_irq_handler(uart0_rx_ringbuf_handler);
#else
    uart0_set_rx_irq_handler(uart0_rx_simple_handler);
#endif

    spid_ds3234.baseAddress = EUSCI_SPI_BASE_ADDR;
    spid_ds3234.cs_low = ds3234_cs_low;
    spid_ds3234.cs_high = ds3234_cs_high;

    ds3234_init();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
#ifdef LED_SYSTEM_STATES
    sig4_on;
#else
    sig4_off;
#endif

    eh_register(&uart0_rx_handler, SYS_MSG_UART0_RX);
    eh_register(&ds3234_irq_handler, SYS_MSG_P53_INT);
    display_menu();

    while (1) {
        // sleep
#ifdef LED_SYSTEM_STATES
        sig4_off;
#endif
        _BIS_SR(LPM3_bits + GIE);
#ifdef LED_SYSTEM_STATES
        sig4_on;
#endif
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        //led_switch;
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

