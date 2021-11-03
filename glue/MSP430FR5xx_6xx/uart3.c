
#include <msp430.h>
#include <inttypes.h>
#include <string.h>

#ifdef USE_UART3
#include "clock_selection.h"
#include "uart_config.h"
#include "uart3.h"
#include "lib_ringbuf.h"

static uint8_t (*uart3_rx_irq_handler)(const uint8_t c);
static uint8_t (*uart3_tx_irq_handler)(void);

uint8_t uart3_rx_buf[UART3_RXBUF_SZ];     // receive buffer
volatile uint8_t uart3_p;       // number of characters received, 0 if none
volatile uint8_t uart3_rx_enable;
volatile uint8_t uart3_rx_err;

#if defined(UART3_RX_USES_RINGBUF) 
struct ringbuf uart3_rbrx;
#endif

#if defined(UART3_TX_USES_IRQ) 
struct ringbuf uart3_rbtx;
uint8_t uart3_tx_buf[UART3_TXBUF_SZ];     // receive buffer
volatile uint8_t uart3_tx_busy;
#endif

volatile uint8_t uart3_last_event;



// you'll have to initialize/map uart ports in main()
// or use uart3_port_init() if no mapping is needed

void uart3_init(void)
{
    UCA3CTLW0 = UCSWRST;        // put eUSCI state machine in reset

#if defined(UC_CTLW0)
    UCA3CTLW0 |= UC_CTLW0;

    #if UART3_BAUD == 9600
    UCA3BRW = BRW_9600_BAUD;
    UCA3MCTLW = MCTLW_9600_BAUD;

    #elif UART3_BAUD == 19200
    UCA3BRW = BRW_19200_BAUD;
    UCA3MCTLW = MCTLW_19200_BAUD;

    #elif UART3_BAUD == 38400
    UCA3BRW = BRW_38400_BAUD;
    UCA3MCTLW = MCTLW_38400_BAUD;

    #elif UART3_BAUD == 57600
    UCA3BRW = BRW_57600_BAUD;
    UCA3MCTLW = MCTLW_57600_BAUD;

    #elif UART3_BAUD == 115200
    UCA3BRW = BRW_115200_BAUD;
    UCA3MCTLW = MCTLW_115200_BAUD;

    #endif

#else
    //#error baud not defined
    // a 9600 BAUD based on ACLK
    UCA3CTLW0 |= UCSSEL__ACLK;
    UCA3BRW = 3;
    UCA3MCTLW |= 0x9200;
#endif

    UCA3CTLW0 &= ~UCSWRST;      // Initialize eUSCI

#ifdef UART3_TX_USES_IRQ
    ringbuf_init(&uart3_rbtx, uart3_tx_buf, UART3_TXBUF_SZ);
    UCA3IE |= UCRXIE | UCTXIE;           // Enable USCI_A0 interrupts
    //UCA3IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
    //UCA3IE |= UCTXIE;
    //UCA3IFG &= ~UCTXIFG;
    uart3_tx_busy = 0;
#else
    UCA3IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
#endif

    uart3_p = 0;
    uart3_rx_enable = 1;
    uart3_rx_err = 0;

#ifdef UART3_RX_USES_RINGBUF
    ringbuf_init(&uart3_rbrx, uart3_rx_buf, UART3_RXBUF_SZ);
#endif

    //uart3_set_rx_irq_handler(uart3_rx_simple_handler);
}

void uart3_initb(const uint8_t baudrate)
{
    UCA3CTLW0 = UCSWRST;        // put eUSCI state machine in reset

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

    switch (baudrate) {
#if defined(UART3_BAUD_9600)
        case BAUDRATE_9600:
            UCA3CTLW0 |= UC_CTLW0;
            UCA3BRW = BRW_9600_BAUD;
            UCA3MCTLW = MCTLW_9600_BAUD;
            break;
#endif
#if defined(UART3_BAUD_19200)
        case BAUDRATE_19200:
            UCA3CTLW0 |= UC_CTLW0;
            UCA3BRW = BRW_19200_BAUD;
            UCA3MCTLW = MCTLW_19200_BAUD;
            break;
#endif
#if defined(UART3_BAUD_38400)
        case BAUDRATE_38400:
            UCA3CTLW0 |= UC_CTLW0;
            UCA3BRW = BRW_38400_BAUD;
            UCA3MCTLW = MCTLW_38400_BAUD;
            break;
#endif
#if defined(UART3_BAUD_19200)
        case BAUDRATE_57600:
            UCA3CTLW0 |= UC_CTLW0;
            UCA3BRW = BRW_57600_BAUD;
            UCA3MCTLW = MCTLW_57600_BAUD;
            break;
#endif
#if defined(UART3_BAUD_115200)
        case BAUDRATE_115200:
            UCA3CTLW0 |= UC_CTLW0;
            UCA3BRW = BRW_115200_BAUD;
            UCA3MCTLW = MCTLW_115200_BAUD;
            break;
#endif
    }

    UCA3CTLW0 &= ~UCSWRST;      // Initialize eUSCI
    UCA3IE |= UCRXIE;           // Enable USCI_A3 RX interrupt

    uart3_p = 0;
    uart3_rx_enable = 1;
    uart3_rx_err = 0;
}

// default port locations
void uart3_port_init(void)
{
    P6SEL0 |= (BIT0 | BIT1);
    P6SEL1 &= ~(BIT0 | BIT1);
}

void uart3_set_rx_irq_handler(uint8_t (*input)(const uint8_t c))
{
    uart3_rx_irq_handler = input;
}

#if defined (UART3_RX_USES_RINGBUF)

// function returns 1 if the main loop should be woken up
uint8_t uart3_rx_ringbuf_handler(const uint8_t c)
{
    if (!ringbuf_put(&uart3_rbrx, c)) {
        // the ringbuffer is full
        uart3_rx_err++;
    }

    if (c == 0x0d) {
        return 1;
    }

    return 0;
}

#else
uint8_t uart3_rx_simple_handler(const uint8_t c)
{

    if (c == 0x0a) {
        return 0;
    }

    if (uart3_rx_enable && (!uart3_rx_err) && (uart3_p < UART3_RXBUF_SZ)) {
        if (c == 0x0d) {
            uart3_rx_buf[uart3_p] = 0;
            uart3_rx_enable = 0;
            uart3_rx_err = 0;
            return 1;
            //ev = UART3_EV_RX;
            //_BIC_SR_IRQ(LPM3_bits);
        } else {
            uart3_rx_buf[uart3_p] = c;
            uart3_p++;
        }
    } else {
        uart3_rx_err++;
        uart3_p = 0;
        if (c == 0x0d) {
            uart3_rx_err = 0;
            uart3_rx_enable = 1;
        }
    }
    /*
    if (rx == 'a') {
        UCA3TXBUF = '_';
    }
    */
    //uart3_tx_str((const char *)&rx, 1);
    return 0;
}

void uart3_set_eol(void)
{
    uart3_p = 0;
    uart3_rx_enable = 1;
}
#endif


void uart3_set_tx_irq_handler(uint8_t (*output)(void))
{
    uart3_tx_irq_handler = output;
}

uint8_t uart3_get_event(void)
{
    return uart3_last_event;
}

void uart3_rst_event(void)
{
    uart3_last_event = UART3_EV_NULL;
}

#ifdef UART3_RX_USES_RINGBUF
struct ringbuf *uart3_get_rx_ringbuf(void)
{
    return &uart3_rbrx;
}
#else
char *uart3_get_rx_buf(void)
{
    if (uart3_p) {
        return (char *)uart3_rx_buf;
    } else {
        return NULL;
    }
}
#endif

#ifdef UART3_TX_USES_IRQ
void uart3_tx_activate()
{
    uint8_t t;

    if (!uart3_tx_busy) {
        if (ringbuf_get(&uart3_rbtx, &t)) {
            uart3_tx_busy = 1;
            UCA3TXBUF = t;
        }
    }
}

void uart3_tx(const uint8_t byte)
{
    if (ringbuf_put(&uart3_rbtx, byte)) {
    }

    uart3_tx_activate();
}

uint16_t uart3_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;

    while (p < size) {        
        if (ringbuf_put(&uart3_rbtx, str[p])) {
            p++;
        }
        if (p == 1) {
            uart3_tx_activate();
        }
    }
    //uart3_tx_activate();
    return p;
}

uint16_t uart3_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        if (ringbuf_put(&uart3_rbtx, str[p])) {
            p++;
        }
        if (p == 1) {
            uart3_tx_activate();
        }
    }
    //uart3_tx_activate();
    return p;
}

#else
void uart3_tx(const uint8_t byte)
{
    while (!(UCA3IFG & UCTXIFG)) {
    }                       // USCI_A0 TX buffer ready?
    UCA3TXBUF = byte;
}

uint16_t uart3_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;
    while (p < size) {
        while (!(UCA3IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        UCA3TXBUF = str[p];
        p++;
    }
    return p;
}

uint16_t uart3_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        while (!(UCA3IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        UCA3TXBUF = str[p];
        p++;
    }
    return p;
}
#endif

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A3_VECTOR))) USCI_A3_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA3IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART3_TX_USES_IRQ
    uint8_t t;
    //int16_t rb;
#endif

#ifdef LED_SYSTEM_STATES
    sig3_on;
#endif

    switch (iv) {
    case USCI_UART_UCRXIFG:
        if (UCA3STATW & UCRXERR) {
            // clear error flags by forcing a dummy read
            r = UCA3RXBUF;
        } else {
            r = UCA3RXBUF;
            //UCA3TXBUF = r; // local echo
            if (uart3_rx_irq_handler != NULL) {
                if (uart3_rx_irq_handler(r)) {
                    ev |= UART3_EV_RX;
                    LPM3_EXIT;
                }
            }
        }
        break;
    case USCI_UART_UCTXIFG:
#ifdef UART3_TX_USES_IRQ
        if (ringbuf_get(&uart3_rbtx, &t)) {
            uart3_tx_busy = 1;
            UCA3TXBUF = t;
        } else {
            // nothing more to do
            uart3_tx_busy = 0;
        }
#endif
        break;
    default:
        break;
    }
    uart3_last_event |= ev;

#ifdef LED_SYSTEM_STATES
    sig3_off;
#endif
}

#endif

