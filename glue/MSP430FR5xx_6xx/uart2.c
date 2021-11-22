
#include <msp430.h>
#include <inttypes.h>
#include <string.h>

#ifdef USE_UART2
#include "clock_selection.h"
#include "uart_config.h"
#include "uart2.h"
#include "uart2_pin.h"
#include "lib_ringbuf.h"

static uint8_t (*uart2_rx_irq_handler)(const uint8_t c);
static uint8_t (*uart2_tx_irq_handler)(void);

uint8_t uart2_rx_buf[UART2_RXBUF_SZ];     // receive buffer
volatile uint8_t uart2_p;       // number of characters received, 0 if none
volatile uint8_t uart2_rx_enable;
volatile uint8_t uart2_rx_err;

#if defined(UART2_RX_USES_RINGBUF) 
struct ringbuf uart2_rbrx;
#endif

#if defined(UART2_TX_USES_IRQ) 
struct ringbuf uart2_rbtx;
uint8_t uart2_tx_buf[UART2_TXBUF_SZ];     // receive buffer
volatile uint8_t uart2_tx_busy;
#endif

volatile uint8_t uart2_last_event;



// you'll have to initialize/map uart ports in main()
// or use uart2_port_init() if no mapping is needed

void uart2_init(void)
{
    UCA2CTLW0 = UCSWRST;        // put eUSCI state machine in reset

#if defined(UC_CTLW0)

    #if UART2_BAUD == 9600
    UCA2CTLW0 |= UC_CTLW0;
    UCA2BRW = BRW_9600_BAUD;
    UCA2MCTLW = MCTLW_9600_BAUD;

    #elif UART2_BAUD == 19200
    UCA2CTLW0 |= UC_CTLW0;
    UCA2BRW = BRW_19200_BAUD;
    UCA2MCTLW = MCTLW_19200_BAUD;

    #elif UART2_BAUD == 38400
    UCA2CTLW0 |= UC_CTLW0;
    UCA2BRW = BRW_38400_BAUD;
    UCA2MCTLW = MCTLW_38400_BAUD;

    #elif UART2_BAUD == 57600
    UCA2CTLW0 |= UC_CTLW0;
    UCA2BRW = BRW_57600_BAUD;
    UCA2MCTLW = MCTLW_57600_BAUD;

    #elif UART2_BAUD == 115200
    UCA2CTLW0 |= UC_CTLW0;
    UCA2BRW = BRW_115200_BAUD;
    UCA2MCTLW = MCTLW_115200_BAUD;

    #else
    #error UART2_BAUD not defined
    // a 9600 BAUD based on ACLK
    //UCA2CTLW0 |= UCSSEL__ACLK;
    //UCA2BRW = 3;
    //UCA2MCTLW |= 0x9200;
    #endif
#endif

    UCA2CTLW0 &= ~UCSWRST;      // Initialize eUSCI

#ifdef UART2_TX_USES_IRQ
    ringbuf_init(&uart2_rbtx, uart2_tx_buf, UART2_TXBUF_SZ);
    UCA2IE |= UCRXIE | UCTXIE;           // Enable USCI_A0 interrupts
    //UCA2IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
    //UCA2IE |= UCTXIE;
    //UCA2IFG &= ~UCTXIFG;
    uart2_tx_busy = 0;
#else
    UCA2IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
#endif

    uart2_p = 0;
    uart2_rx_enable = 1;
    uart2_rx_err = 0;

#ifdef UART2_RX_USES_RINGBUF
    ringbuf_init(&uart2_rbrx, uart2_rx_buf, UART2_RXBUF_SZ);
#endif

    //uart2_set_rx_irq_handler(uart2_rx_simple_handler);
}

void uart2_initb(const uint8_t baudrate)
{
    UCA2CTLW0 = UCSWRST;        // put eUSCI state machine in reset

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

    switch (baudrate) {
        case BAUDRATE_9600:
            UCA2CTLW0 |= UC_CTLW0;
            UCA2BRW = BRW_9600_BAUD;
            UCA2MCTLW = MCTLW_9600_BAUD;
            break;
        case BAUDRATE_19200:
            UCA2CTLW0 |= UC_CTLW0;
            UCA2BRW = BRW_19200_BAUD;
            UCA2MCTLW = MCTLW_19200_BAUD;
            break;
        case BAUDRATE_38400:
            UCA2CTLW0 |= UC_CTLW0;
            UCA2BRW = BRW_38400_BAUD;
            UCA2MCTLW = MCTLW_38400_BAUD;
            break;
        case BAUDRATE_57600:
            UCA2CTLW0 |= UC_CTLW0;
            UCA2BRW = BRW_57600_BAUD;
            UCA2MCTLW = MCTLW_57600_BAUD;
            break;
        case BAUDRATE_115200:
            UCA2CTLW0 |= UC_CTLW0;
            UCA2BRW = BRW_115200_BAUD;
            UCA2MCTLW = MCTLW_115200_BAUD;
            break;
    }

    UCA2CTLW0 &= ~UCSWRST;      // Initialize eUSCI
    UCA2IE |= UCRXIE;           // Enable USCI_A2 RX interrupt

    uart2_p = 0;
    uart2_rx_enable = 1;
    uart2_rx_err = 0;
}

// default port locations
void uart2_port_init(void)
{
    uart2_pin_init();
}

void uart2_set_rx_irq_handler(uint8_t (*input)(const uint8_t c))
{
    uart2_rx_irq_handler = input;
}

#if defined (UART2_RX_USES_RINGBUF)

// function returns 1 if the main loop should be woken up
uint8_t uart2_rx_ringbuf_handler(const uint8_t c)
{
    if (!ringbuf_put(&uart2_rbrx, c)) {
        // the ringbuffer is full
        uart2_rx_err++;
    }

    if (c == 0x0d) {
        return 1;
    }

    return 0;
}

#else
uint8_t uart2_rx_simple_handler(const uint8_t c)
{

    if (c == 0x0a) {
        return 0;
    }

    if (uart2_rx_enable && (!uart2_rx_err) && (uart2_p < UART2_RXBUF_SZ)) {
        if (c == 0x0d) {
            uart2_rx_buf[uart2_p] = 0;
            uart2_rx_enable = 0;
            uart2_rx_err = 0;
            return 1;
            //ev = UART2_EV_RX;
            //_BIC_SR_IRQ(LPM3_bits);
        } else {
            uart2_rx_buf[uart2_p] = c;
            uart2_p++;
        }
    } else {
        uart2_rx_err++;
        uart2_p = 0;
        if (c == 0x0d) {
            uart2_rx_err = 0;
            uart2_rx_enable = 1;
        }
    }
    /*
    if (rx == 'a') {
        UCA2TXBUF = '_';
    }
    */
    //uart2_tx_str((const char *)&rx, 1);
    return 0;
}

void uart2_set_eol(void)
{
    uart2_p = 0;
    uart2_rx_enable = 1;
}
#endif


void uart2_set_tx_irq_handler(uint8_t (*output)(void))
{
    uart2_tx_irq_handler = output;
}

uint8_t uart2_get_event(void)
{
    return uart2_last_event;
}

void uart2_rst_event(void)
{
    uart2_last_event = UART2_EV_NULL;
}

#ifdef UART2_RX_USES_RINGBUF
struct ringbuf *uart2_get_rx_ringbuf(void)
{
    return &uart2_rbrx;
}
#else
char *uart2_get_rx_buf(void)
{
    if (uart2_p) {
        return (char *)uart2_rx_buf;
    } else {
        return NULL;
    }
}
#endif

#ifdef UART2_TX_USES_IRQ
void uart2_tx_activate()
{
    uint8_t t;

    if (!uart2_tx_busy) {
        if (ringbuf_get(&uart2_rbtx, &t)) {
            uart2_tx_busy = 1;
            UCA2TXBUF = t;
        }
    }
}

void uart2_tx(const uint8_t byte)
{
    while (ringbuf_put(&uart2_rbtx, byte) == 0) {
        // wait for the ring buffer to clear
        uart2_tx_activate();
    }

    uart2_tx_activate();
}

uint16_t uart2_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;

    while (p < size) {        
        if (ringbuf_put(&uart2_rbtx, str[p])) {
            p++;
            uart2_tx_activate();
        }
    }
    return p;
}

uint16_t uart2_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        if (ringbuf_put(&uart2_rbtx, str[p])) {
            p++;
            uart2_tx_activate();
        }
    }
    return p;
}

#else
void uart2_tx(const uint8_t byte)
{
    while (!(UCA2IFG & UCTXIFG)) {
    }                       // USCI_A0 TX buffer ready?
    UCA2TXBUF = byte;
}

uint16_t uart2_tx_str(const char *str, const uint16_t size)
{
    uint16_t p = 0;
    while (p < size) {
        while (!(UCA2IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        UCA2TXBUF = str[p];
        p++;
    }
    return p;
}

uint16_t uart2_print(const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        while (!(UCA2IFG & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        UCA2TXBUF = str[p];
        p++;
    }
    return p;
}
#endif

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A2_VECTOR))) USCI_A2_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA2IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART2_TX_USES_IRQ
    uint8_t t;
    //int16_t rb;
#endif

#ifdef LED_SYSTEM_STATES
    sig3_on;
#endif

    switch (iv) {
    case USCI_UART_UCRXIFG:
        if (UCA2STATW & UCRXERR) {
            // clear error flags by forcing a dummy read
            r = UCA2RXBUF;
        } else {
            r = UCA2RXBUF;
            //UCA2TXBUF = r; // local echo
            if (uart2_rx_irq_handler != NULL) {
                if (uart2_rx_irq_handler(r)) {
                    ev |= UART2_EV_RX;
                    LPM3_EXIT;
                }
            }
        }
        break;
    case USCI_UART_UCTXIFG:
#ifdef UART2_TX_USES_IRQ
        if (ringbuf_get(&uart2_rbtx, &t)) {
            uart2_tx_busy = 1;
            UCA2TXBUF = t;
        } else {
            // nothing more to do
            uart2_tx_busy = 0;
        }
#endif
        break;
    default:
        break;
    }
    uart2_last_event |= ev;

#ifdef LED_SYSTEM_STATES
    sig3_off;
#endif
}

#endif

