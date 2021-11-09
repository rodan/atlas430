
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
#include "hi_stirrer.h"
#include "qa.h"

hi_stirrer_t st;
volatile uint8_t spi_sm_state = HIST_smIDLE;
volatile uint8_t spi_sm_counter = 0;
volatile uint8_t t_debug = 1;

void main_init(void)
{
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);

    P1OUT = 0;
    P1DIR = 0xfb;

    P5OUT = 0;
    P5DIR = 0xf7;

    // temp test for errata USCI47
    P5OUT |= BIT2;

    P5SEL1 &= ~(BIT0 | BIT1 | BIT2);        // USCI_B1 SCLK, MOSI, 
    P5SEL0 |= (BIT0 | BIT1 | BIT2);         // and MISO pin

    // IRQ triggers on rising edge
    P5IES &= ~CS_PIN;
    // Reset IRQ flags
    P5IFG &= ~CS_PIN;
    // Enable interrupt
    P5IE |= CS_PIN;

#ifdef USE_XT1
    PJSEL0 |= BIT4 | BIT5;
    CS_setExternalClockSource(32768, 0);
#endif

    // Set DCO Frequency to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    // Set DCO Frequency to 1MHz
    //CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_0);

    // configure MCLK, SMCLK to be sourced by DCOCLK
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

#ifdef USE_XT1
    CS_turnOnLFXT(CS_LFXT_DRIVE_3);
#endif
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

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();

    uart0_port_init();
    uart0_init();

    hist_struct_init(&st);

    // Configure USCI_B1 for SPI operation
    UCB1CTLW0 = UCSWRST;                    // **Put state machine in reset**
                                            // 4-pin, 8-bit SPI slave
    // remove UCCKPH due to errata USCI47 - slaz681
    UCB1CTLW0 |= UCSYNC | UCCKPL | UCMSB; // | UCMODE_2 | UCSTEM;
                                            // Clock polarity high, MSB
    UCB1CTLW0 |= UCSSEL__SMCLK;             // ACLK
    UCB1BRW =  0x8; // 0x50;  // 8Mhz/80 == 100khz
    UCB1CTLW0 &= ~UCSWRST;                  // **Initialize USCI state machine**
    UCB1IE |= UCRXIE;                       // Enable USCI_B1 RX interrupt

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    led_off;

    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);
    display_menu();


    while (1) {
        //while (!(UCB1IFG & UCTXIFG)) { };  // USCI_B1 TX buffer ready?
        //UCB1TXBUF = 0x99; //spi_sm_counter+6;//*(mem_map+spi_sm_counter);

        // sleep
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        //led_switch;
        check_events();
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_B1_VECTOR))) USCI_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{

    uint8_t rxbyte;
    uint8_t *mem_map = (uint8_t *) &st;

    switch (__even_in_range(UCB1IV, USCI_SPI_UCTXIFG)) {
        case USCI_NONE: 
            //while (!(UCB1IFG & UCTXIFG)) { };  // USCI_B1 TX buffer ready?
            //UCB1TXBUF = 0x88; //spi_sm_counter+6;//*(mem_map+spi_sm_counter);
        break;
        case USCI_SPI_UCRXIFG:
            rxbyte = UCB1RXBUF; // clear interrupt
            if (!(P5IN & CS_PIN)) {
                // parse the input only if CS is active (low)

                switch (spi_sm_state) {
                    case HIST_smIDLE:
                        //uart0_print("i ");
                        // first byte came in
                        if (rxbyte & HIST_bmWRITE) {
                            spi_sm_state = HIST_smWRITE;
                        } else {
                            spi_sm_state = HIST_smREAD;
                            spi_sm_counter = rxbyte & 0x7f;  // last 7 bits contain the address
                            while (!(UCB1IFG & UCTXIFG)) { };  // USCI_B1 TX buffer ready?
                            if (spi_sm_counter < 0x20) {
                                UCB1TXBUF = *(mem_map+spi_sm_counter);
                            } else {
                                // reading over bounds, send fake zeroes
                                UCB1TXBUF = 0xdd;//HIST_bFAKEDATA;
                            }
                            spi_sm_counter++;
                        }
                        break;
                    case HIST_smREAD:
                        //uart0_print("r");
                        while (!(UCB1IFG & UCTXIFG)) { };  // USCI_B1 TX buffer ready?
                        if (spi_sm_counter < 0x20) {
                            //UCB1TXBUF = spi_sm_counter;
                            UCB1TXBUF = *(mem_map+spi_sm_counter);
                        } else {
                            // reading over bounds, send fake zeroes
                            UCB1TXBUF = 0xbb;//HIST_bFAKEDATA;
                        }
                        spi_sm_counter++;
                        break;
                    case HIST_smWRITE:
                        //uart0_print("w");
                        break;
                }
                break;
            }
        case USCI_SPI_UCTXIFG:
            //while (!(UCB1IFG & UCTXIFG)) { };  // USCI_B1 TX buffer ready?
            //UCB1TXBUF = 0x77; //spi_sm_counter+6;// *(mem_map+spi_sm_counter);
        break;
        default: 
            //while (!(UCB1IFG & UCTXIFG)) { };  // USCI_B1 TX buffer ready?
            //UCB1TXBUF = 0x66; //spi_sm_counter+6;// *(mem_map+spi_sm_counter);
        break;
        //}
        }
        
#if 0
        while (!(UCB1IFG&UCTXIFG)) { };         // USCI_B1 TX buffer ready?
        UCB1TXBUF = t_debug;                  // Echo received data
        t_debug++;
        //while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_SPI_BASE_ADDR, EUSCI_A_SPI_TRANSMIT_INTERRUPT)) { };
        //EUSCI_A_SPI_transmitData(EUSCI_SPI_BASE_ADDR, transmitData);
#endif

}

// Port 5 interrupt service routine - in place of the hardware CS logic
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
#elif defined(__GNUC__)
__attribute__ ((interrupt(PORT5_VECTOR)))
void Port5_ISR(void)
#else
#error Compiler not supported!
#endif
{
    led_switch;
    if (P5IFG & CS_PIN) {
        spi_sm_state = HIST_smIDLE;
        spi_sm_counter = 0;
        P5IFG &= ~CS_PIN;
        t_debug = 1;

        // FIXME reset the spi bus
        //UCB1IE &= ~UCRXIE; //clear RX interrupt
        //UCB1CTLW0 = UCSWRST;   // **Put state machine in reset**
        //UCB1CTLW0 &= ~UCSWRST; // **Initialize USCI state machine**
        //UCB1IE |= UCRXIE;      // Enable USCI_B1 RX interrupt
    }
}

