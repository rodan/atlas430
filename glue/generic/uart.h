#ifndef __UART_H__
#define __UART_H__

#include <msp430.h>
#include "lib_ringbuf.h"

#define       CH_UCA0  0x1
#define       CH_UCA1  0x2
#define       CH_UCA2  0x4
#define       CH_UCA3  0x8

#define  UART_EV_NULL  0
#define    UART_EV_RX  0x1
#define    UART_EV_TX  0x2


#ifdef UART_TX_USES_RINGBUF
#ifndef UART_TX_USES_IRQ
#define UART_TX_USES_IRQ
#endif
#endif

typedef struct _uart_descriptor {
    uint16_t baseAddress;   ///< uart subsystem offset base register
    uint8_t baudrate;       ///< select one of the supported baudrates [BAUDRATE_9600 - BAUDRATE_115200]
    uint8_t channel;        ///< one of UCA0 UCA1 UCA2 UCA3, gets populated automatically
    uint8_t last_event;     ///< event flag set during irq handler for main loop 
    uint8_t tx_busy;        ///< true if tx ringbuf is currently sending out data
    uint8_t p;              ///< number of characters received, 0 if none
    uint8_t rx_enable;      ///< true if buffers not yet full (rx is allowed)
    uint8_t rx_err;         ///< error counter

#if defined(UART_RX_USES_RINGBUF)
    struct ringbuf rbrx;    ///< incoming ring buffer structure
#endif
    uint8_t rx_buf[UART_RXBUF_SZ];  ///< incoming buffer

#if defined(UART_TX_USES_IRQ)
    struct ringbuf rbtx;     ///< outgoing ring buffer structure
    uint8_t tx_buf[UART_TXBUF_SZ];  ///< outgoing buffer
#endif
} uart_descriptor; ///< struct that needs to be populated before the library calls are made


#ifdef UART_USES_UCA0
    #if defined (EUSCI_A0_VECTOR)
        #define HAL_USCI_A0_VECTOR EUSCI_A0_VECTOR
    #elif defined (USCI_A0_VECTOR)
        #define HAL_USCI_A0_VECTOR USCI_A0_VECTOR
    #else
        #error UART UCA0 interrupt vector name unknown
    #endif

    #if defined (OFS_UCA0STATW)
        #define HAL_UCA0STAT UCA0STATW
    #else
        #define HAL_UCA0STAT UCA0STAT
    #endif

#endif

#ifdef UART_USES_UCA1
    #if defined (EUSCI_A1_VECTOR)
        #define HAL_USCI_A1_VECTOR EUSCI_A1_VECTOR
    #elif defined (USCI_A1_VECTOR)
        #define HAL_USCI_A1_VECTOR USCI_A1_VECTOR
    #else
        #error UART UCA1 interrupt vector name unknown
    #endif

    #if defined (OFS_UCA1STATW)
        #define HAL_UCA1STAT UCA1STATW
    #else
        #define HAL_UCA1STAT UCA1STAT
    #endif

#endif

#ifdef UART_USES_UCA2
    #if defined (EUSCI_A2_VECTOR)
        #define HAL_USCI_A2_VECTOR EUSCI_A2_VECTOR
    #elif defined (USCI_A2_VECTOR)
        #define HAL_USCI_A2_VECTOR USCI_A2_VECTOR
    #else
        #error UART UCA2 interrupt vector name unknown
    #endif

    #if defined (OFS_UCA2STATW)
        #define HAL_UCA2STAT UCA2STATW
    #else
        #define HAL_UCA2STAT UCA2STAT
    #endif

#endif

#ifdef UART_USES_UCA3
    #if defined (EUSCI_A3_VECTOR)
        #define HAL_USCI_A3_VECTOR EUSCI_A3_VECTOR
    #elif defined (USCI_A3_VECTOR)
        #define HAL_USCI_A3_VECTOR USCI_A3_VECTOR
    #else
        #error UART UCA3 interrupt vector name unknown
    #endif

    #if defined (OFS_UCA3STATW)
        #define HAL_UCA3STAT UCA3STATW
    #else
        #define HAL_UCA3STAT UCA3STAT
    #endif

#endif

// possible interrupt names
// sometimes EUSCI-capable uCs define the interrupt vector as USCI_Ax_VECTOR instead of EUSCI_Ax_VECTOR

// possible interrupt vector flags
#if defined (USCI_UCRXIFG)
    #define HAL_IV_UCRXIFG USCI_UCRXIFG
#elif defined (USCI_UART_UCRXIFG)
    #define HAL_IV_UCRXIFG USCI_UART_UCRXIFG
#else
    #error RX flag for UART interrupt vector unknown
#endif
#if defined (USCI_UCTXIFG)
    #define HAL_IV_UCTXIFG USCI_UCTXIFG
#elif defined (USCI_UART_UCTXIFG)
    #define HAL_IV_UCTXIFG USCI_UART_UCTXIFG
#else
    #error TX flag for UART interrupt vector unknown
#endif

// UCASTAT register

#ifdef __cplusplus
extern "C" {
#endif

uint8_t uart_init(uart_descriptor *uartd);
void uart_pin_init(const uart_descriptor *uartd);

uint8_t uart_set_rx_irq_handler(const uart_descriptor *uartd, uint8_t (*input)(uart_descriptor *uartd, const uint8_t c));

// sample inside-irq handlers
uint8_t uart_rx_ringbuf_handler(uart_descriptor *uartd, const uint8_t c);
uint8_t uart_rx_simple_handler(uart_descriptor *uartd, const uint8_t c);

uint8_t uart_get_event(const uart_descriptor *uartd);
void uart_rst_event(uart_descriptor *uartd);

void uart_tx(uart_descriptor *uartd, const uint8_t byte);
uint16_t uart_tx_str(uart_descriptor *uartd, const char *str, const uint16_t size);
uint16_t uart_print(uart_descriptor *uartd, const char *str);

#ifdef UART_RX_USES_RINGBUF
struct ringbuf *uart_get_rx_ringbuf(uart_descriptor *uartd);
#define uart_set_eol(a)
#else
char *uart_get_rx_buf(const uart_descriptor *uartd);
void uart_set_eol(uart_descriptor *uartd);
#endif

#ifdef __cplusplus
}
#endif

#endif
