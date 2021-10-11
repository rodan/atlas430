
#include <msp430.h>
#include <inttypes.h>
#include <string.h>
#include "uart1.h"

volatile char uart1_rx_buf[UART1_RXBUF_SZ];     // receive buffer
volatile uint8_t uart1_p;       // number of characters received, 0 if none
volatile uint8_t uart1_rx_enable;
volatile uint8_t uart1_rx_err;

volatile uint8_t uart1_last_event;

// you'll have to initialize/map uart ports in main()
// or use uart1_port_init() if no mapping is needed
void uart1_init(void)
{
    UCA1CTLW0 = UCSWRST;        // put eUSCI state machine in reset

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

#ifdef UART1_SPEED_9600_1M
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 6;
    UCA1MCTLW = 0x2081;
#elif defined (UART1_SPEED_19200_1M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 3;
    UCA1MCTLW = 0x0241;
#elif defined (UART1_SPEED_38400_1M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 1;
    UCA1MCTLW = 0x00a1;
#elif defined (UART1_SPEED_57600_1M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 17;
    UCA1MCTLW = 0x4a00;
#elif defined (UART1_SPEED_115200_1M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 8;
    UCA1MCTLW = 0xd600;
#elif defined (UART1_SPEED_9600_8M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 52;
    UCA1MCTLW = 0x4911;
#elif defined (UART1_SPEED_19200_8M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 26;
    UCA1MCTLW = 0xb601;
#elif defined (UART1_SPEED_38400_8M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 13;
    UCA1MCTLW = 0x8401;
#elif defined (UART1_SPEED_57600_8M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 8;
    UCA1MCTLW = 0xf7a1;
#elif defined (UART1_SPEED_115200_8M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 4;
    UCA1MCTLW = 0x5551;
#elif defined (UART1_SPEED_230400_8M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 2;
    UCA1MCTLW = 0xbb21;
#elif defined (UART1_SPEED_460800_8M)
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 17;
    UCA1MCTLW = 0x4a00;
#else // a safer default of 9600 - does not depend on SMCLK
    UCA1CTLW0 |= UCSSEL__ACLK;
    UCA1BRW = 3;
    //UCA1MCTLW |= 0x5300;
    UCA1MCTLW |= 0x9200;
#endif

    UCA1CTLW0 &= ~UCSWRST;      // Initialize eUSCI
    UCA1IE |= UCRXIE;           // Enable USCI_A1 RX interrupt

    uart1_p = 0;
    uart1_rx_enable = 1;
    uart1_rx_err = 0;
}

// default port locations
void uart1_port_init(void)
{
    P2SEL0 &= ~(BIT0 | BIT1);
    P2SEL1 |= (BIT0 | BIT1);
}

uint8_t uart1_get_event(void)
{
    return uart1_last_event;
}

void uart1_rst_event(void)
{
    uart1_last_event = UART1_EV_NULL;
}

void uart1_set_eol(void)
{
    uart1_p = 0;
    uart1_rx_enable = 1;
}

char *uart1_get_rx_buf(void)
{
    if (uart1_p) {
        return (char *)uart1_rx_buf;
    } else {
        return NULL;
    }
}

uint16_t uart1_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;
    while (p < size) {
        while (!(UCA1IFG & UCTXIFG)) {
        }                       // USCI_A1 TX buffer ready?
        UCA1TXBUF = str[p];
        p++;
    }
    return p;
}

uint16_t uart1_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        while (!(UCA1IFG & UCTXIFG)) {
        }                       // USCI_A1 TX buffer ready?
        UCA1TXBUF = str[p];
        p++;
    }
    return p;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A1_VECTOR))) USCI_A1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA1IV;
    register char rx;
    uint8_t ev = 0;

    switch (iv) {
    case USCI_UART_UCRXIFG:
        rx = UCA1RXBUF;

        if (rx == 0x0a) {
            return;
        }

        if (uart1_rx_enable && (!uart1_rx_err) && (uart1_p < UART1_RXBUF_SZ)) {
            if (rx == 0x0d) {
                uart1_rx_buf[uart1_p] = 0;
                uart1_rx_enable = 0;
                uart1_rx_err = 0;
                if (uart1_p) {
                    ev = UART1_EV_RX;
                    _BIC_SR_IRQ(LPM3_bits);
                }
            } else {
                uart1_rx_buf[uart1_p] = rx;
                uart1_p++;
            }
        } else {
            uart1_rx_err++;
            uart1_p = 0;
            if (rx == 0x0d) {
                uart1_rx_err = 0;
                uart1_rx_enable = 1;
            }
        }
        break;
    case USCI_UART_UCTXIFG:
        ev = UART1_EV_TX;
        break;
    default:
        break;
    }
    uart1_last_event |= ev;
}
