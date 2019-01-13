
#include "uart1.h"

void uart1_init(void)
{
    // hardware UART
    UCA1CTL1 |= UCSWRST;        // put state machine in reset
    UCA1CTL1 |= UCSSEL_1;       // use ACLK
    UCA1BR0 = 0x03;             // 32kHz/9600=3.41
    UCA1BR1 = 0x00;
    UCA1MCTL = UCBRS_3 + UCBRF_0;       // modulation UCBRSx=3, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;       // initialize USCI state machine
    UCA1IE |= UCRXIE;           // enable USCI_A0 RX interrupt
    uart1_p = 0;
    uart1_rx_enable = 1;
    uart1_rx_err = 0;
}

uint16_t uart1_tx_str(char *str, uint16_t size)
{
    uint16_t p = 0;
    while (p < size) {
        while (!(UCA1IFG & UCTXIFG)) ;  // USCI_A0 TX buffer ready?
        UCA1TXBUF = str[p];
        p++;
    }
    return p;
}

__attribute__ ((interrupt(USCI_A1_VECTOR)))
void USCI_A1_ISR(void)
{
    uint16_t iv = UCA1IV;
    register char rx;

    enum uart1_tevent ev = UART1_EV_NONE;

    // iv is 2 for RXIFG, 4 for TXIFG
    switch (iv) {
    case 2:
        rx = UCA1RXBUF;
        if (uart1_rx_enable && !uart1_rx_err && (uart1_p < UART1_RXBUF_SZ-2)) {
            if (rx == 0x0d) {
                ev = UART1_EV_RX;
                uart1_rx_buf[uart1_p] = 0;
                uart1_rx_enable = 0;
                uart1_rx_err = 0;
                _BIC_SR_IRQ(LPM3_bits);
            } else if (rx == 0x0a) {
                if (uart1_p != 0) {
                    ev = UART1_EV_RX;
                    uart1_rx_buf[uart1_p] = 0;
                    uart1_rx_enable = 0;
                    uart1_rx_err = 0;
                    _BIC_SR_IRQ(LPM3_bits);
                }
            } else {
                uart1_rx_buf[uart1_p] = rx;
                uart1_p++;
            }
        } else {
            uart1_rx_err++;
            if ((rx == 0x0d) || (rx == 0x0a)) {
                uart1_rx_err = 0;
                uart1_p = 0;
            }
        }

        break;
    case 4:
        ev = UART1_EV_TX;
        break;
    default:
        break;
    }
    uart1_last_event |= ev;
}
