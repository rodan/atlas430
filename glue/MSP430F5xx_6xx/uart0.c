/*
  uart0 init functions
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430
*/

#include <msp430.h>
#include <inttypes.h>
#include <string.h>

#ifdef USE_UART0
#include "clock_selection.h"
#include "uart_config.h"
#include "uart0.h"
#include "uart0_pin.h"
#include "lib_ringbuf.h"

#ifdef USE_SIG
#include "sig.h"
#endif

static uint8_t (*uart0_rx_irq_handler)(const uint8_t c);
static uint8_t (*uart0_tx_irq_handler)(void);

uint8_t uart0_rx_buf[UART0_RXBUF_SZ];     // receive buffer
volatile uint8_t uart0_p;       // number of characters received, 0 if none
volatile uint8_t uart0_rx_enable;
volatile uint8_t uart0_rx_err;

#if defined(UART0_RX_USES_RINGBUF) 
struct ringbuf uart0_rbrx;
#endif

#if defined(UART0_TX_USES_IRQ) 
struct ringbuf uart0_rbtx;
uint8_t uart0_tx_buf[UART0_TXBUF_SZ];     // receive buffer
volatile uint8_t uart0_tx_busy;
#endif

volatile uint8_t uart0_last_event;

// you'll have to initialize/map uart ports in main()
// or use uart0_pin_init() if no remapping is needed

void uart0_init(void)
{
    UCA0CTL1 |= UCSWRST;        // put eUSCI state machine in reset

#if defined(UC_CTL1)

    #if UART0_BAUD == 9600
    UCA0CTL1 |= UC_CTL1;
    UCA0BR0 = BR_9600_BAUD & 0xff;
    UCA0BR1 = (BR_9600_BAUD >> 8) & 0xff;
    UCA0MCTL = MCTL_9600_BAUD;

    #elif UART0_BAUD == 19200
    UCA0CTL1 |= UC_CTL1;
    UCA0BR0 = BR_19200_BAUD & 0xff;
    UCA0BR1 = (BR_19200_BAUD >> 8) & 0xff;
    UCA0MCTL = MCTL_19200_BAUD;

    #elif UART0_BAUD == 38400
    UCA0CTL1 |= UC_CTL1;
    UCA0BR0 = BR_38400_BAUD & 0xff;
    UCA0BR1 = (BR_38400_BAUD >> 8) & 0xff;
    UCA0MCTL = MCTL_38400_BAUD;

    #elif UART0_BAUD == 57600
    UCA0CTL1 |= UC_CTL1;
    UCA0BR0 = BR_57600_BAUD & 0xff;
    UCA0BR1 = (BR_57600_BAUD >> 8) & 0xff;
    UCA0MCTL = MCTL_57600_BAUD;

    #elif UART0_BAUD == 115200
    UCA0CTL1 |= UC_CTL1;
    UCA0BR0 = BR_115200_BAUD & 0xff;
    UCA0BR1 = (BR_115200_BAUD >> 8) & 0xff;
    UCA0MCTL = MCTL_115200_BAUD;

    #else
    #error UART0_BAUD not defined
    // a 9600 BAUD based on ACLK
    //UCA0CTL1 |= UCSSEL__ACLK;
    //UCA0BR1 = 3;
    //UCA0MCTL |= UCBRS_3;
    #endif

#endif

    UCA0CTL1 &= ~UCSWRST;      // Initialize eUSCI

#ifdef UART0_TX_USES_IRQ
    ringbuf_init(&uart0_rbtx, uart0_tx_buf, UART0_TXBUF_SZ);
    UCA0IE |= UCRXIE | UCTXIE;           // Enable USCI_A0 interrupts
    uart0_tx_busy = 0;
#else
    UCA0IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
#endif

    uart0_p = 0;
    uart0_rx_enable = 1;
    uart0_rx_err = 0;

#ifdef UART0_RX_USES_RINGBUF
    ringbuf_init(&uart0_rbrx, uart0_rx_buf, UART0_RXBUF_SZ);
#endif
}

void uart0_initb(const uint8_t baudrate)
{
    UCA0CTL1 = UCSWRST;        // put eUSCI state machine in reset

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

    switch (baudrate) {
        case BAUDRATE_9600:
            UCA0CTL1 |= UC_CTL1;
            UCA0BR0 = BR_9600_BAUD & 0xff;
            UCA0BR1 = (BR_9600_BAUD >> 8) & 0xff;
            UCA0MCTL = MCTL_9600_BAUD;
            break;
        case BAUDRATE_19200:
            UCA0CTL1 |= UC_CTL1;
            UCA0BR0 = BR_19200_BAUD & 0xff;
            UCA0BR1 = (BR_19200_BAUD >> 8) & 0xff;
            UCA0MCTL = MCTL_19200_BAUD;
            break;
        case BAUDRATE_38400:
            UCA0CTL1 |= UC_CTL1;
            UCA0BR0 = BR_38400_BAUD & 0xff;
            UCA0BR1 = (BR_38400_BAUD >> 8) & 0xff;
            UCA0MCTL = MCTL_38400_BAUD;
            break;
        case BAUDRATE_57600:
            UCA0CTL1 |= UC_CTL1;
            UCA0BR0 = BR_57600_BAUD & 0xff;
            UCA0BR1 = (BR_57600_BAUD >> 8) & 0xff;
            UCA0MCTL = MCTL_57600_BAUD;
            break;
        case BAUDRATE_115200:
            UCA0CTL1 |= UC_CTL1;
            UCA0BR0 = BR_115200_BAUD & 0xff;
            UCA0BR1 = (BR_115200_BAUD >> 8) & 0xff;
            UCA0MCTL = MCTL_115200_BAUD;
            break;
    }

    UCA0CTL1 &= ~UCSWRST;      // Initialize eUSCI

#ifdef UART0_TX_USES_IRQ
    ringbuf_init(&uart0_rbtx, uart0_tx_buf, UART0_TXBUF_SZ);
    UCA0IE |= UCRXIE | UCTXIE;           // Enable USCI_A0 interrupts
    uart0_tx_busy = 0;
#else
    UCA0IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
#endif

    uart0_p = 0;
    uart0_rx_enable = 1;
    uart0_rx_err = 0;

#ifdef UART0_RX_USES_RINGBUF
    ringbuf_init(&uart0_rbrx, uart0_rx_buf, UART0_RXBUF_SZ);
#endif
}

void uart0_port_init(void)
{
    uart0_pin_init();
}

void uart0_set_rx_irq_handler(uint8_t (*input)(const uint8_t c))
{
    uart0_rx_irq_handler = input;
}

#if defined (UART0_RX_USES_RINGBUF)

// function returns 1 if the main loop should be woken up
uint8_t uart0_rx_ringbuf_handler(const uint8_t c)
{
    if (!ringbuf_put(&uart0_rbrx, c)) {
        // the ringbuffer is full
        uart0_rx_err++;
    }

    if (c == 0x0d) {
        return 1;
    }

    return 0;
}

#else
uint8_t uart0_rx_simple_handler(const uint8_t c)
{

    if (c == 0x0a) {
        return 0;
    }

    if (uart0_rx_enable && (!uart0_rx_err) && (uart0_p < UART0_RXBUF_SZ)) {
        if (c == 0x0d) {
            uart0_rx_buf[uart0_p] = 0;
            uart0_rx_enable = 0;
            uart0_rx_err = 0;
            return 1;
            //ev = UART0_EV_RX;
            //_BIC_SR_IRQ(LPM3_bits);
        } else {
            uart0_rx_buf[uart0_p] = c;
            uart0_p++;
        }
    } else {
        uart0_rx_err++;
        uart0_p = 0;
        if (c == 0x0d) {
            uart0_rx_err = 0;
            uart0_rx_enable = 1;
        }
    }
    //uart0_tx_str((const char *)&rx, 1);
    return 0;
}

void uart0_set_eol(void)
{
    uart0_p = 0;
    uart0_rx_enable = 1;
}
#endif


void uart0_set_tx_irq_handler(uint8_t (*output)(void))
{
    uart0_tx_irq_handler = output;
}

uint8_t uart0_get_event(void)
{
    return uart0_last_event;
}

void uart0_rst_event(void)
{
    uart0_last_event = UART0_EV_NULL;
}

#ifdef UART0_RX_USES_RINGBUF
struct ringbuf *uart0_get_rx_ringbuf(void)
{
    return &uart0_rbrx;
}
#else
char *uart0_get_rx_buf(void)
{
    if (uart0_p) {
        return (char *)uart0_rx_buf;
    } else {
        return NULL;
    }
}
#endif

#ifdef UART0_TX_USES_IRQ
void uart0_tx_activate()
{
    uint8_t t;

    if (!uart0_tx_busy) {
        if (ringbuf_get(&uart0_rbtx, &t)) {
            uart0_tx_busy = 1;
            UCA0TXBUF = t;
        } else {
            // nothing more to do
            uart0_tx_busy = 0;
        }
    }
}

void uart0_tx(const uint8_t byte)
{
    while (ringbuf_put(&uart0_rbtx, byte) == 0) {
        // wait for the ring buffer to clear
        uart0_tx_activate();
#ifdef UART_TX_USES_LPM
        _BIS_SR(LPM0_bits + GIE);
#endif
    }

    uart0_tx_activate();
}

uint16_t uart0_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;

    while (p < size) {        
        if (ringbuf_put(&uart0_rbtx, str[p])) {
            p++;
            uart0_tx_activate();
        }
#ifdef UART_TX_USES_LPM
        _BIS_SR(LPM0_bits + GIE);
#endif
    }
    return p;
}

uint16_t uart0_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        if (ringbuf_put(&uart0_rbtx, str[p])) {
            p++;
            uart0_tx_activate();
        }
#ifdef UART_TX_USES_LPM
        _BIS_SR(LPM0_bits + GIE);
#endif
    }
    return p;
}

#else
void uart0_tx(const uint8_t byte)
{
    while (!(UCA0IFG & UCTXIFG)) {
    }                       // USCI_A0 TX buffer ready?
    UCA0TXBUF = byte;
}

uint16_t uart0_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;
    while (p < size) {
        while (!(UCA0IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        UCA0TXBUF = str[p];
        p++;
    }
    return p;
}

uint16_t uart0_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        while (!(UCA0IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        UCA0TXBUF = str[p];
        p++;
    }
    return p;
}
#endif

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA0IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART0_TX_USES_IRQ
    uint8_t t;
#endif

#ifdef USE_SIG
    sig3_on;
#endif

    switch (iv) {
    case USCI_UCRXIFG:
        if (UCA0STAT & UCRXERR) {
            // clear error flags by forcing a dummy read
            r = UCA0RXBUF;
        } else {
            r = UCA0RXBUF;
            //UCA0TXBUF = r; // local echo
            if (uart0_rx_irq_handler != NULL) {
                if (uart0_rx_irq_handler(r)) {
                    ev |= UART0_EV_RX;
                    LPM3_EXIT;
                }
            }
        }
        break;
    case USCI_UCTXIFG:
#ifdef UART0_TX_USES_IRQ
        if (ringbuf_get(&uart0_rbtx, &t)) {
            uart0_tx_busy = 1;
            UCA0TXBUF = t;
        } else {
            // nothing more to do
            uart0_tx_busy = 0;
        }
        LPM3_EXIT;
#endif
        break;
    default:
        break;
    }
    uart0_last_event |= ev;

#ifdef USE_SIG
    sig3_off;
#endif
}

#endif

