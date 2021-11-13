
#include <msp430.h>
#include <inttypes.h>
#include <string.h>

#ifdef USE_UART1
#include "clock_selection.h"
#include "uart_config.h"
#include "uart1.h"
#include "lib_ringbuf.h"

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
// or use uart1_port_init() if no mapping is needed

void uart1_init(void)
{
    UCA1CTLW0 = UCSWRST;        // put eUSCI state machine in reset

#if defined(UC_CTLW0)

    #if UART1_BAUD == 9600
    UCA1CTLW0 |= UC_CTLW0;
    UCA1BRW = BRW_9600_BAUD;
    UCA1MCTLW = MCTLW_9600_BAUD;

    #elif UART1_BAUD == 19200
    UCA1CTLW0 |= UC_CTLW0;
    UCA1BRW = BRW_19200_BAUD;
    UCA1MCTLW = MCTLW_19200_BAUD;

    #elif UART1_BAUD == 38400
    UCA1CTLW0 |= UC_CTLW0;
    UCA1BRW = BRW_38400_BAUD;
    UCA1MCTLW = MCTLW_38400_BAUD;

    #elif UART1_BAUD == 57600
    UCA1CTLW0 |= UC_CTLW0;
    UCA1BRW = BRW_57600_BAUD;
    UCA1MCTLW = MCTLW_57600_BAUD;

    #elif UART1_BAUD == 115200
    UCA1CTLW0 |= UC_CTLW0;
    UCA1BRW = BRW_115200_BAUD;
    UCA1MCTLW = MCTLW_115200_BAUD;

    #else
    #error UART1_BAUD not defined
    // a 9600 BAUD based on ACLK
    //UCA1CTLW0 |= UCSSEL__ACLK;
    //UCA1BRW = 3;
    //UCA1MCTLW |= 0x9200;
    #endif
#endif

    UCA1CTLW0 &= ~UCSWRST;      // Initialize eUSCI

#ifdef UART1_TX_USES_IRQ
    ringbuf_init(&uart1_rbtx, uart1_tx_buf, UART1_TXBUF_SZ);
    UCA1IE |= UCRXIE | UCTXIE;           // Enable USCI_A0 interrupts
    //UCA1IE |= UCRXIE;           // Enable USCI_A0 RX interrupt
    //UCA1IE |= UCTXIE;
    //UCA1IFG &= ~UCTXIFG;
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

    //uart1_set_rx_irq_handler(uart1_rx_simple_handler);
}

void uart1_initb(const uint8_t baudrate)
{
    UCA1CTLW0 = UCSWRST;        // put eUSCI state machine in reset

    // consult 'Recommended Settings for Typical Crystals and Baud Rates' in slau367o
    // for some reason any baud >= 115200 ends up with a non-working RX channel

    switch (baudrate) {
        case BAUDRATE_9600:
            UCA1CTLW0 |= UC_CTLW0;
            UCA1BRW = BRW_9600_BAUD;
            UCA1MCTLW = MCTLW_9600_BAUD;
            break;
        case BAUDRATE_19200:
            UCA1CTLW0 |= UC_CTLW0;
            UCA1BRW = BRW_19200_BAUD;
            UCA1MCTLW = MCTLW_19200_BAUD;
            break;
        case BAUDRATE_38400:
            UCA1CTLW0 |= UC_CTLW0;
            UCA1BRW = BRW_38400_BAUD;
            UCA1MCTLW = MCTLW_38400_BAUD;
            break;
        case BAUDRATE_57600:
            UCA1CTLW0 |= UC_CTLW0;
            UCA1BRW = BRW_57600_BAUD;
            UCA1MCTLW = MCTLW_57600_BAUD;
            break;
        case BAUDRATE_115200:
            UCA1CTLW0 |= UC_CTLW0;
            UCA1BRW = BRW_115200_BAUD;
            UCA1MCTLW = MCTLW_115200_BAUD;
            break;
    }

    UCA1CTLW0 &= ~UCSWRST;      // Initialize eUSCI
    UCA1IE |= UCRXIE;           // Enable USCI_A1 RX interrupt

    uart1_p = 0;
    uart1_rx_enable = 1;
    uart1_rx_err = 0;
}

// default port locations
void uart1_port_init(void)
{
#if defined (__MSP430FR5969__)
    P2SEL0 &= ~(BIT5 | BIT6);
    P2SEL1 |= BIT5 | BIT6;
#elif defined (__MSP430FR5994__)
    P6SEL0 |= BIT0 | BIT1;
    P6SEL1 &= ~(BIT0 | BIT1);
#elif defined (__MSP430FR6989__)
    P3SEL0 |= BIT4 | BIT5;
    P3SEL1 &= ~(BIT4 | BIT5);
#else
    #error "UCA1 pins unknown for this uC, please modify glue/MSP430FR5xx_6xx/uart1.c"
#endif
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
    /*
    if (rx == 'a') {
        UCA1TXBUF = '_';
    }
    */
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
        }
    }
}

void uart1_tx(const uint8_t byte)
{
    while (ringbuf_put(&uart1_rbtx, byte) == 0) {
        // wait for the ring buffer to clear
        uart1_tx_activate();
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

// sometimes EUSCI-capable uCs define the interrupt vector as USCI_Ax_VECTOR instead of EUSCI_Ax_VECTOR
#if defined (EUSCI_A1_VECTOR)

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A1_VECTOR))) USCI_A1_ISR(void)
#else
#error Compiler not supported!
#endif

#elif defined (USCI_A1_VECTOR)

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR(void)
#else
#error Compiler not supported!
#endif

#else
    #error "can't find interrupt vector for USCI_A1"
#endif

{
    uint16_t iv = UCA1IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART1_TX_USES_IRQ
    uint8_t t;
    //int16_t rb;
#endif

#ifdef LED_SYSTEM_STATES
    sig3_on;
#endif

    switch (iv) {
    case USCI_UART_UCRXIFG:
        if (UCA1STATW & UCRXERR) {
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
    case USCI_UART_UCTXIFG:
#ifdef UART1_TX_USES_IRQ
        if (ringbuf_get(&uart1_rbtx, &t)) {
            uart1_tx_busy = 1;
            UCA1TXBUF = t;
        } else {
            // nothing more to do
            uart1_tx_busy = 0;
        }
#endif
        break;
    default:
        break;
    }
    uart1_last_event |= ev;

#ifdef LED_SYSTEM_STATES
    sig3_off;
#endif
}

#endif

