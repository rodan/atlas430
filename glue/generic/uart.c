/*
  uart0 init functions
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430
*/

#include <msp430.h>
#include <inttypes.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "clock_selection.h"
#include "uart_config.h"
#include "uart.h"
#include "uart_uca0_pin.h"
#include "uart_uca1_pin.h"
#include "uart_uca2_pin.h"
#include "uart_uca3_pin.h"
#include "lib_ringbuf.h"

#ifdef USE_SIG
#include "sig.h"
#endif

uint8_t (*uart0_rx_irq_handler)(uart_descriptor *uartd, const uint8_t c);
uart_descriptor *uart0d;

uint8_t (*uart1_rx_irq_handler)(uart_descriptor *uartd, const uint8_t c);
uart_descriptor *uart1d;

uint8_t (*uart2_rx_irq_handler)(uart_descriptor *uartd, const uint8_t c);
uart_descriptor *uart2d;

uint8_t (*uart3_rx_irq_handler)(uart_descriptor *uartd, const uint8_t c);
uart_descriptor *uart3d;


// you'll have to initialize/map uart pins in main()
// or use uart_pin_init() if no remapping is needed

uint8_t uart_init(uart_descriptor *uartd)
{
    uint8_t ret = STATUS_SUCCESS;

    // set up optional ring buffers
#ifdef UART_TX_USES_IRQ
    ringbuf_init(&uartd->rbtx, uartd->tx_buf, UART_TXBUF_SZ);
    uartd->tx_busy = 0;
#endif

#ifdef UART_RX_USES_RINGBUF
    ringbuf_init(&uartd->rbrx, uartd->rx_buf, UART_RXBUF_SZ);
#endif

    // set up registers
    uart_config_reg(uartd->baseAddress, uartd->baudrate);

    // reset counters
    uartd->p = 0;
    uartd->rx_enable = 1;
    uartd->rx_err = 0;

    // determine USCI channel based on baseAddress
    switch (uartd->baseAddress) {
#if defined (USCI_A0_BASE)
        case USCI_A0_BASE:
            uartd->channel = CH_UCA0;
            uart0d = uartd;
            break;
#endif
#if defined (USCI_A1_BASE)
        case USCI_A1_BASE:
            uartd->channel = CH_UCA1;
            uart1d = uartd;
            break;
#endif
#if defined (USCI_A2_BASE)
        case USCI_A2_BASE:
            uartd->channel = CH_UCA2;
            uart2d = uartd;
            break;
#endif
#if defined (USCI_A3_BASE)
        case USCI_A3_BASE:
            uartd->channel = CH_UCA3;
            uart3d = uartd;
            break;
#endif
#if defined (EUSCI_A0_BASE)
        case EUSCI_A0_BASE:
            uartd->channel = CH_UCA0;
            uart0d = uartd;
            break;
#endif
#if defined (EUSCI_A1_BASE)
        case EUSCI_A1_BASE:
            uartd->channel = CH_UCA1;
            uart1d = uartd;
            break;
#endif
#if defined (EUSCI_A2_BASE)
        case EUSCI_A2_BASE:
            uartd->channel = CH_UCA2;
            uart2d = uartd;
            break;
#endif
#if defined (EUSCI_A3_BASE)
        case EUSCI_A3_BASE:
            uartd->channel = CH_UCA3;
            uart3d = uartd;
            break;
#endif
        default:
            ret = STATUS_FAIL;
            break;
    }

    return ret;
}

uint8_t uart_pin_init(const uart_descriptor *uartd)
{
    uint8_t ret = STATUS_SUCCESS;

    switch (uartd->baseAddress) {
#if defined (USCI_A0_BASE)
        case USCI_A0_BASE:
            uart_uca0_pin_init();
            break;
#endif
#if defined (USCI_A1_BASE)
        case USCI_A1_BASE:
            uart_uca1_pin_init();
            break;
#endif
#if defined (USCI_A2_BASE)
        case USCI_A2_BASE:
            uart_uca2_pin_init();
            break;
#endif
#if defined (USCI_A3_BASE)
        case USCI_A3_BASE:
            uart_uca3_pin_init();
            break;
#endif
#if defined (EUSCI_A0_BASE)
        case EUSCI_A0_BASE:
            uart_uca0_pin_init();
            break;
#endif
#if defined (EUSCI_A1_BASE)
        case EUSCI_A1_BASE:
            uart_uca1_pin_init();
            break;
#endif
#if defined (EUSCI_A2_BASE)
        case EUSCI_A2_BASE:
            uart_uca2_pin_init();
            break;
#endif
#if defined (EUSCI_A3_BASE)
        case EUSCI_A3_BASE:
            uart_uca3_pin_init();
            break;
#endif
        default:
            ret = STATUS_FAIL;
            break;
    }

    return ret;
}

uint8_t uart_set_rx_irq_handler(const uart_descriptor *uartd, uint8_t (*input)(uart_descriptor *uartd, const uint8_t c))
{
    uint8_t ret = STATUS_SUCCESS;

    switch (uartd->channel) {
        case CH_UCA0:
            uart0_rx_irq_handler = input;
            break;
        case CH_UCA1:
            uart1_rx_irq_handler = input;
            break;
        case CH_UCA2:
            uart2_rx_irq_handler = input;
            break;
        case CH_UCA3:
            uart3_rx_irq_handler = input;
            break;
        default:
            ret = STATUS_FAIL;
            break;
    }

    return ret;
}

#if defined (UART_RX_USES_RINGBUF)

// function returns 1 if the main loop should be woken up
uint8_t uart_rx_ringbuf_handler(uart_descriptor *uartd, const uint8_t c)
{
    if (!ringbuf_put(&uartd->rbrx, c)) {
        // the ringbuffer is full
        uartd->rx_err++;
        // FIXME maybe return 1 here?
    }

    if (c == 0x0d) {
        return 1;
    }

    return 0;
}

#else
uint8_t uart_rx_simple_handler(uart_descriptor *uartd, const uint8_t c)
{

    if (c == 0x0a) {
        return 0;
    }

    if (uartd->rx_enable && (!uartd->rx_err) && (uartd->p < UART_RXBUF_SZ)) {
        if (c == 0x0d) {
            uartd->rx_buf[uartd->p] = 0;
            uartd->rx_enable = 0;
            uartd->rx_err = 0;
            return 1;
            //ev = UART_EV_RX;
            //_BIC_SR_IRQ(LPM3_bits);
        } else {
            uartd->rx_buf[uartd->p] = c;
            uartd->p++;
        }
    } else {
        uartd->rx_err++;
        uartd->p = 0;
        if (c == 0x0d) {
            uartd->rx_err = 0;
            uartd->rx_enable = 1;
        }
    }

    //uart0_tx_str((const char *)&rx, 1);
    return 0;
}

void uart_set_eol(uart_descriptor *uartd)
{
    uartd->p = 0;
    uartd->rx_enable = 1;
}
#endif

uint8_t uart_get_event(const uart_descriptor *uartd)
{
    return uartd->last_event;
}

void uart_rst_event(uart_descriptor *uartd)
{
    uartd->last_event = UART_EV_NULL;
}

#ifdef UART_RX_USES_RINGBUF
struct ringbuf *uart_get_rx_ringbuf(uart_descriptor *uartd)
{
    return &uartd->rbrx;
}
#else
char *uart_get_rx_buf(const uart_descriptor *uartd)
{
    if (uartd->p) {
        return (char *)uartd->rx_buf;
    } else {
        return NULL;
    }
}
#endif

#ifdef UART_TX_USES_IRQ
void uart_tx_activate(uart_descriptor *uartd)
{
    uint8_t t;

    if (!uartd->tx_busy) {
        if (ringbuf_get(&uartd->rbtx, &t)) {
            uartd->tx_busy = 1;
            HWREG8(uartd->baseAddress + OFS_UCAxTXBUF) = t;
        } else {
            // nothing more to do
            uartd->tx_busy = 0;
        }
    }
}

void uart_tx(uart_descriptor *uartd, const uint8_t byte)
{
    while (ringbuf_put(&uartd->rbtx, byte) == 0) {
        // wait for the ring buffer to clear
        uart_tx_activate(uartd);
        _BIS_SR(LPM0_bits + GIE);
    }

    uart_tx_activate(uartd);
}

uint16_t uart_tx_str(uart_descriptor *uartd, const char *str, const uint16_t size)
{
    uint16_t p = 0;

    while (p < size) {        
        if (ringbuf_put(&uartd->rbtx, str[p])) {
            p++;
            uart_tx_activate(uartd);
        }
        _BIS_SR(LPM0_bits + GIE);
    }
    return p;
}

uint16_t uart_print(uart_descriptor *uartd, const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        if (ringbuf_put(&uartd->rbtx, str[p])) {
            p++;
            uart_tx_activate(uartd);
        }
        _BIS_SR(LPM0_bits + GIE);
    }
    return p;
}

#else
void uart_tx(uart_descriptor *uartd, const uint8_t byte)
{
    while (!(HWREG8(uartd->baseAddress + OFS_UCAxIFG) & UCTXIFG)) {
    }                       // USCI_A0 TX buffer ready?
    HWREG8(uartd->baseAddress + OFS_UCAxTXBUF) = byte;
}

uint16_t uart_tx_str(uart_descriptor *uartd, const char *str, const uint16_t size)
{
    uint16_t p = 0;
    while (p < size) {
        while (!(HWREG8(uartd->baseAddress + OFS_UCAxIFG) & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        HWREG8(uartd->baseAddress + OFS_UCAxTXBUF) = str[p];
        p++;
    }
    return p;
}

uint16_t uart_print(uart_descriptor *uartd, const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        while (!(HWREG8(uartd->baseAddress + OFS_UCAxIFG) & UCTXIFG)) {
        }                       // USCI_A0 TX buffer ready?
        HWREG8(uartd->baseAddress + OFS_UCAxTXBUF) = str[p];
        p++;
    }
    return p;
}

#endif


// interrupt vectors

#ifdef UART_USES_UCA0

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector=HAL_USCI_A0_VECTOR
    __interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(HAL_USCI_A0_VECTOR))) USCI_A0_ISR(void)
#else
    #error Compiler not supported!
#endif
{
    uint16_t iv = UCA0IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART_TX_USES_IRQ
    uint8_t t;
#endif

#ifdef USE_SIG
    sig3_on;
#endif

    switch (iv) {
    case HAL_IV_UCRXIFG:
        if (HAL_UCA0STAT & UCRXERR) {
            // clear error flags by forcing a dummy read
            r = UCA0RXBUF;
        } else {
            r = UCA0RXBUF;
            //UCA0TXBUF = r; // local echo
            if (uart0_rx_irq_handler != NULL) {
                if (uart0_rx_irq_handler(uart0d, r)) {
                    ev |= UART_EV_RX;
                    LPM3_EXIT;
                }
            }
        }
        break;
    case HAL_IV_UCTXIFG:
#ifdef UART_TX_USES_IRQ
        if (ringbuf_get(&uart0d->rbtx, &t)) {
            uart0d->tx_busy = 1;
            UCA0TXBUF = t;
        } else {
            // nothing more to do
            uart0d->tx_busy = 0;
        }
        LPM3_EXIT;
#endif
        break;
    default:
        break;
    }
    uart0d->last_event |= ev;

#ifdef USE_SIG
    sig3_off;
#endif
}

#endif



#ifdef UART_USES_UCA1

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=HAL_USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(HAL_USCI_A1_VECTOR))) USCI_A1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA1IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART_TX_USES_IRQ
    uint8_t t;
#endif

#ifdef USE_SIG
    sig3_on;
#endif

    switch (iv) {
    case HAL_IV_UCRXIFG:
        if (HAL_UCA1STAT & UCRXERR) {
            // clear error flags by forcing a dummy read
            r = UCA1RXBUF;
        } else {
            r = UCA1RXBUF;
            //UCA1TXBUF = r; // local echo
            if (uart1_rx_irq_handler != NULL) {
                if (uart1_rx_irq_handler(uart1d, r)) {
                    ev |= UART_EV_RX;
                    LPM3_EXIT;
                }
            }
        }
        break;
    case HAL_IV_UCTXIFG:
#ifdef UART_TX_USES_IRQ
        if (ringbuf_get(&uart1d->rbtx, &t)) {
            uart1d->tx_busy = 1;
            UCA1TXBUF = t;
        } else {
            // nothing more to do
            uart1d->tx_busy = 0;
        }
        LPM3_EXIT;
#endif
        break;
    default:
        break;
    }
    uart1d->last_event |= ev;

#ifdef USE_SIG
    sig3_off;
#endif
}

#endif



#ifdef UART_USES_UCA2

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=HAL_USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(HAL_USCI_A2_VECTOR))) USCI_A2_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA2IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART_TX_USES_IRQ
    uint8_t t;
#endif

#ifdef USE_SIG
    sig3_on;
#endif

    switch (iv) {
    case HAL_IV_UCRXIFG:
        if (HAL_UCA2STAT & UCRXERR) {
            // clear error flags by forcing a dummy read
            r = UCA2RXBUF;
        } else {
            r = UCA2RXBUF;
            //UCA2TXBUF = r; // local echo
            if (uart2_rx_irq_handler != NULL) {
                if (uart2_rx_irq_handler(uart2d, r)) {
                    ev |= UART_EV_RX;
                    LPM3_EXIT;
                }
            }
        }
        break;
    case HAL_IV_UCTXIFG:
#ifdef UART_TX_USES_IRQ
        if (ringbuf_get(&uart2d->rbtx, &t)) {
            uart2d->tx_busy = 1;
            UCA2TXBUF = t;
        } else {
            // nothing more to do
            uart2d->tx_busy = 0;
        }
        LPM3_EXIT;
#endif
        break;
    default:
        break;
    }
    uart2d->last_event |= ev;

#ifdef USE_SIG
    sig3_off;
#endif
}

#endif



#ifdef UART_USES_UCA3

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=HAL_USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(HAL_USCI_A3_VECTOR))) USCI_A3_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = UCA3IV;
    register char r;
    uint8_t ev = 0;
#ifdef UART_TX_USES_IRQ
    uint8_t t;
#endif

#ifdef USE_SIG
    sig3_on;
#endif

    switch (iv) {
    case HAL_IV_UCRXIFG:
        if (HAL_UCA3STAT & UCRXERR) {
            // clear error flags by forcing a dummy read
            r = UCA3RXBUF;
        } else {
            r = UCA3RXBUF;
            //UCA3TXBUF = r; // local echo
            if (uart3_rx_irq_handler != NULL) {
                if (uart3_rx_irq_handler(uart3d, r)) {
                    ev |= UART_EV_RX;
                    LPM3_EXIT;
                }
            }
        }
        break;
    case HAL_IV_UCTXIFG:
#ifdef UART_TX_USES_IRQ
        if (ringbuf_get(&uart3d->rbtx, &t)) {
            uart3d->tx_busy = 1;
            UCA3TXBUF = t;
        } else {
            // nothing more to do
            uart3d->tx_busy = 0;
        }
        LPM3_EXIT;
#endif
        break;
    default:
        break;
    }
    uart3d->last_event |= ev;

#ifdef USE_SIG
    sig3_off;
#endif
}

#endif

