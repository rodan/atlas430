/*
  uart1 init functions
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430
*/

#include <msp430.h>
#include <inttypes.h>
#include <string.h>

#ifdef USE_UART1
#include "clock_selection.h"
#include "uart_config.h"
#include "uart1.h"
#include "uart1_pin.h"
#include "lib_ringbuf.h"

#ifdef USE_SIG
#include "sig.h"
#endif

static uint8_t (*uart1_rx_irq_handler)(const uint8_t c);
static uint8_t (*uart1_tx_irq_handler)(void);

uint8_t uart1_rx_buf[UART1_RXBUF_SZ];     // receive buffer
volatile uint8_t uart1_p;       // number of characters received, 0 if none
volatile uint8_t uart1_rx_enable;
volatile uint8_t uart1_rx_err;

#if defined(UART1_RX_USES_RINGBUF) 
struct ringbuf uart1_rbrx;
#endif

#if defined(UART1_TX_USES_IRQ) 
struct ringbuf uart1_rbtx;
uint8_t uart1_tx_buf[UART1_TXBUF_SZ];     // receive buffer
volatile uint8_t uart1_tx_busy;
#endif

volatile uint8_t uart1_last_event;

// you'll have to initialize/map uart ports in main()
// or use uart1_pin_init() if no remapping is needed

void uart1_init(void)
{
    UCA1CTL1 |= UCSWRST;        // put eUSCI state machine in reset

#if defined(UC_CTL1)

    #if UART1_BAUD == 9600
    UCA1CTL1 |= UC_CTL1;
    UCA1BR0 = BR_9600_BAUD & 0xff;
    UCA1BR1 = (BR_9600_BAUD >> 8) & 0xff;
    UCA1MCTL = MCTL_9600_BAUD;

    #elif UART1_BAUD == 19200
    UCA1CTL1 |= UC_CTL1;
    UCA1BR0 = BR_19200_BAUD & 0xff;
    UCA1BR1 = (BR_19200_BAUD >> 8) & 0xff;
    UCA1MCTL = MCTL_19200_BAUD;

    #elif UART1_BAUD == 38400
    UCA1CTL1 |= UC_CTL1;
    UCA1BR0 = BR_38400_BAUD & 0xff;
    UCA1BR1 = (BR_38400_BAUD >> 8) & 0xff;
    UCA1MCTL = MCTL_38400_BAUD;

    #elif UART1_BAUD == 57600
    UCA1CTL1 |= UC_CTL1;
    UCA1BR0 = BR_57600_BAUD & 0xff;
    UCA1BR1 = (BR_57600_BAUD >> 8) & 0xff;
    UCA1MCTL = MCTL_57600_BAUD;

    #elif UART1_BAUD == 115200
    UCA1CTL1 |= UC_CTL1;
    UCA1BR0 = BR_115200_BAUD & 0xff;
    UCA1BR1 = (BR_115200_BAUD >> 8) & 0xff;
    UCA1MCTL = MCTL_115200_BAUD;

    #else
    #error UART1_BAUD not defined
    // a 9600 BAUD based on ACLK
    //UCA1CTL1 |= UCSSEL__ACLK;
    //UCA1BR1 = 3;
    //UCA1MCTL |= UCBRS_3;
    #endif

#endif

    UCA1CTL1 &= ~UCSWRST;      // Initialize eUSCI

#ifdef UART1_TX_USES_IRQ
    ringbuf_init(&uart1_rbtx, uart1_tx_buf, UART1_TXBUF_SZ);
    UCA1IE |= UCRXIE | UCTXIE;           // Enable USCI_A0 interrupts
    uart1_tx_busy = 0;
#else
    UCA1IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
#endif

    uart1_p = 0;
    uart1_rx_enable = 1;
    uart1_rx_err = 0;

#ifdef UART1_RX_USES_RINGBUF
    ringbuf_init(&uart1_rbrx, uart1_rx_buf, UART1_RXBUF_SZ);
#endif
}

void uart1_initb(const uint8_t baudrate)
{
    UCA1CTL1 = UCSWRST;        // put eUSCI state machine in reset

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

    switch (baudrate) {
        case BAUDRATE_9600:
            UCA1CTL1 |= UC_CTL1;
            UCA1BR0 = BR_9600_BAUD & 0xff;
            UCA1BR1 = (BR_9600_BAUD >> 8) & 0xff;
            UCA1MCTL = MCTL_9600_BAUD;
            break;
        case BAUDRATE_19200:
            UCA1CTL1 |= UC_CTL1;
            UCA1BR0 = BR_19200_BAUD & 0xff;
            UCA1BR1 = (BR_19200_BAUD >> 8) & 0xff;
            UCA1MCTL = MCTL_19200_BAUD;
            break;
        case BAUDRATE_38400:
            UCA1CTL1 |= UC_CTL1;
            UCA1BR0 = BR_38400_BAUD & 0xff;
            UCA1BR1 = (BR_38400_BAUD >> 8) & 0xff;
            UCA1MCTL = MCTL_38400_BAUD;
            break;
        case BAUDRATE_57600:
            UCA1CTL1 |= UC_CTL1;
            UCA1BR0 = BR_57600_BAUD & 0xff;
            UCA1BR1 = (BR_57600_BAUD >> 8) & 0xff;
            UCA1MCTL = MCTL_57600_BAUD;
            break;
        case BAUDRATE_115200:
            UCA1CTL1 |= UC_CTL1;
            UCA1BR0 = BR_115200_BAUD & 0xff;
            UCA1BR1 = (BR_115200_BAUD >> 8) & 0xff;
            UCA1MCTL = MCTL_115200_BAUD;
            break;
    }

    UCA1CTL1 &= ~UCSWRST;      // Initialize eUSCI

#ifdef UART1_TX_USES_IRQ
    ringbuf_init(&uart1_rbtx, uart1_tx_buf, UART1_TXBUF_SZ);
    UCA1IE |= UCRXIE | UCTXIE;           // Enable USCI_A0 interrupts
    uart1_tx_busy = 0;
#else
    UCA1IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
#endif

    uart1_p = 0;
    uart1_rx_enable = 1;
    uart1_rx_err = 0;

#ifdef UART1_RX_USES_RINGBUF
    ringbuf_init(&uart1_rbrx, uart1_rx_buf, UART1_RXBUF_SZ);
#endif
}

void uart1_port_init(void)
{
    uart1_pin_init();
}

void uart1_set_rx_irq_handler(uint8_t (*input)(const uint8_t c))
{
    uart1_rx_irq_handler = input;
}

#if defined (UART1_RX_USES_RINGBUF)

// function returns 1 if the main loop should be woken up
uint8_t uart1_rx_ringbuf_handler(const uint8_t c)
{
    if (!ringbuf_put(&uart1_rbrx, c)) {
        // the ringbuffer is full
        uart1_rx_err++;
    }

    if (c == 0x0d) {
        return 1;
    }

    return 0;
}

#else
uint8_t uart1_rx_simple_handler(const uint8_t c)
{

    if (c == 0x0a) {
        return 0;
    }

    if (uart1_rx_enable && (!uart1_rx_err) && (uart1_p < UART1_RXBUF_SZ)) {
        if (c == 0x0d) {
            uart1_rx_buf[uart1_p] = 0;
            uart1_rx_enable = 0;
            uart1_rx_err = 0;
            return 1;
            //ev = UART1_EV_RX;
            //_BIC_SR_IRQ(LPM3_bits);
        } else {
            uart1_rx_buf[uart1_p] = c;
            uart1_p++;
        }
    } else {
        uart1_rx_err++;
        uart1_p = 0;
        if (c == 0x0d) {
            uart1_rx_err = 0;
            uart1_rx_enable = 1;
        }
    }
    //uart1_tx_str((const char *)&rx, 1);
    return 0;
}

void uart1_set_eol(void)
{
    uart1_p = 0;
    uart1_rx_enable = 1;
}
#endif


void uart1_set_tx_irq_handler(uint8_t (*output)(void))
{
    uart1_tx_irq_handler = output;
}

uint8_t uart1_get_event(void)
{
    return uart1_last_event;
}

void uart1_rst_event(void)
{
    uart1_last_event = UART1_EV_NULL;
}

#ifdef UART1_RX_USES_RINGBUF
struct ringbuf *uart1_get_rx_ringbuf(void)
{
    return &uart1_rbrx;
}
#else
char *uart1_get_rx_buf(void)
{
    if (uart1_p) {
        return (char *)uart1_rx_buf;
    } else {
        return NULL;
    }
}
#endif

#ifdef UART1_TX_USES_IRQ
void uart1_tx_activate()
{
    uint8_t t;

    if (!uart1_tx_busy) {
        if (ringbuf_get(&uart1_rbtx, &t)) {
            uart1_tx_busy = 1;
            UCA1TXBUF = t;
        } else {
            // nothing more to do
            uart1_tx_busy = 0;
        }
    }
}

void uart1_tx(const uint8_t byte)
{
    while (ringbuf_put(&uart1_rbtx, byte) == 0) {
        // wait for the ring buffer to clear
        uart1_tx_activate();
#ifdef UART_TX_USES_LPM
        _BIS_SR(LPM0_bits + GIE);
#endif
    }

    uart1_tx_activate();
}

uint16_t uart1_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;

    while (p < size) {        
        if (ringbuf_put(&uart1_rbtx, str[p])) {
            p++;
            uart1_tx_activate();
        }
#ifdef UART_TX_USES_LPM
        _BIS_SR(LPM0_bits + GIE);
#endif
    }
    return p;
}

uint16_t uart1_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        if (ringbuf_put(&uart1_rbtx, str[p])) {
            p++;
            uart1_tx_activate();
        }
#ifdef UART_TX_USES_LPM
        _BIS_SR(LPM0_bits + GIE);
#endif
    }
    return p;
}

#else
void uart1_tx(const uint8_t byte)
{
    while (!(UCA1IFG & UCTXIFG)) {
    }                       // USCI_A0 TX buffer ready?
    UCA1TXBUF = byte;
}

uint16_t uart1_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;
    while (p < size) {
        while (!(UCA1IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
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
        }                       // USCI_A0 TX buffer ready?
        UCA1TXBUF = str[p];
        p++;
    }
    return p;
}
#endif

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA1IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART1_TX_USES_IRQ
    uint8_t t;
#endif

#ifdef USE_SIG
    sig3_on;
#endif

    switch (iv) {
    case USCI_UCRXIFG:
        if (UCA1STAT & UCRXERR) {
            // clear error flags by forcing a dummy read
            r = UCA1RXBUF;
        } else {
            r = UCA1RXBUF;
            //UCA1TXBUF = r; // local echo
            if (uart1_rx_irq_handler != NULL) {
                if (uart1_rx_irq_handler(r)) {
                    ev |= UART1_EV_RX;
                    LPM3_EXIT;
                }
            }
        }
        break;
    case USCI_UCTXIFG:
#ifdef UART1_TX_USES_IRQ
        if (ringbuf_get(&uart1_rbtx, &t)) {
            uart1_tx_busy = 1;
            UCA1TXBUF = t;
        } else {
            // nothing more to do
            uart1_tx_busy = 0;
        }
        LPM3_EXIT;
#endif
        break;
    default:
        break;
    }
    uart1_last_event |= ev;

#ifdef USE_SIG
    sig3_off;
#endif
}

#endif

