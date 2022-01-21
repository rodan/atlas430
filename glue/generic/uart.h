#ifndef __UART_H__
#define __UART_H__

/**
* \addtogroup MOD_UART UART bus
* \brief Application-level UART driver.
* \author Petre Rodan
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_UART
* \author Petre Rodan
**/

#include <msp430.h>
#include "lib_ringbuf.h"

#define       CH_UCA0  0x1
#define       CH_UCA1  0x2
#define       CH_UCA2  0x4
#define       CH_UCA3  0x8

#define  UART_EV_NULL  0    ///< no current UART event has been triggered
#define    UART_EV_RX  0x1  ///< event that signifies that the RX interrupt has been triggered and main loop has been woken up because of it

#ifdef UART_TX_USES_RINGBUF
#ifndef UART_TX_USES_IRQ
#define UART_TX_USES_IRQ
#endif
#endif

// guard needed for firmware that does not use the UART interface at all
#if defined (UART_RXBUF_SZ)


///< struct who's pointer is passed arround during most calls present in this HAL implementation
// do not read/write to the structure elements directly, try to treat this struct as opaque data
// baseAddress and baudrate are the only variables excepted from the above rule. they need to be 
// set before calling uart_init()
// also if you write your own RX interrupt handler callback all elements can be directly read and written
typedef struct _uart_descriptor {
    uint16_t baseAddress;   ///< uart subsystem offset base register
    uint8_t baudrate;       ///< select one of the supported baudrates [BAUDRATE_9600 - BAUDRATE_115200]
    uint8_t channel;        ///< one of UCA0 UCA1 UCA2 UCA3, gets populated automatically based on baseAddress
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
} uart_descriptor;


#ifdef UART_USES_UCA0
    #if defined (EUSCI_A0_VECTOR)
        #define HAL_USCI_A0_VECTOR EUSCI_A0_VECTOR
    #elif defined (USCI_A0_VECTOR)
        #define HAL_USCI_A0_VECTOR USCI_A0_VECTOR
    #else
        #error UART UCA0 interrupt vector name unknown
    #endif

    #if defined (__MSP430_HAS_EUSCI_A0__)
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

    #if defined (__MSP430_HAS_EUSCI_A1__)
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

    #if defined (__MSP430_HAS_EUSCI_A2__)
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

    #if defined (__MSP430_HAS_EUSCI_A3__)
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

#ifdef __cplusplus
extern "C" {
#endif

/** \brief initialize the UART subsystem

    this function initializes the UCAx registers, the ring buffers, counters and the current USCI channel

    \note uartd->baseAddress and uartd->baudrate needs to be set before calling this function

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @return STATUS_SUCCESS or STATUS_FAIL
*/
uint8_t uart_init(uart_descriptor *uartd);

/** \brief initialize the function select registers

    this function initializes the PxSEL registers in order to enable the UART functionality on the target microcontroller

    \note if you get compiler warnings like 
        'warning multiple pins found for the UCA0xXD function, you must initialize them manually'
    means that the micro provides multiple pins for that function and ideally you need to set them up yourself instead of using uart_pin_init(). disregarding the warning enables the uart functionality on all pins with possible magic smoke release. consider yourself warned.

    or errors like:
        'UART_USES_UCAx was defined but pins not known' 
    then most probably the target micro does not have uart functionality on UCAx.
    doublecheck the schematic and the UART_USES_UCAx define. make sure they match.

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @return STATUS_SUCCESS or STATUS_FAIL
*/
uint8_t uart_pin_init(const uart_descriptor *uartd);

/** \brief set callback function to be called from within the RX interrupt handler

    the callback function needs to be small as possible since it's executed by the interrupt vector handler.
    must return 1 if microcontroller need to be woken up after RETI, 0 otherwise

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @param callback function
    @return STATUS_SUCCESS or STATUS_FAIL
*/
uint8_t uart_set_rx_irq_handler(const uart_descriptor *uartd, uint8_t (*input)(uart_descriptor *uartd, const uint8_t c));

/** \brief sample ringbuffer-based callback function to be called from within the RX interrupt handler

    a callback function that manages a ringbuffer and inserts new bytes into it. 
    increments uartd->rx_err and drops the new byte just received if the buffer is full
    wakes up microcontroller if C is newline

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @param c received byte
    @return 1 if microcontroller need to be woken up after RETI, 0 otherwise
*/
uint8_t uart_rx_ringbuf_handler(uart_descriptor *uartd, const uint8_t c);

/** \brief sample linear-buffer based callback function to be called from within the RX interrupt handler

    a callback function that manages a simple buffer and inserts new bytes into it. 
    increments uartd->rx_err, drops the new byte and the entire buffer on error
    wakes up microcontroller if C is newline

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @param c received byte
    @return 1 if microcontroller need to be woken up after RETI, 0 otherwise
*/
uint8_t uart_rx_simple_handler(uart_descriptor *uartd, const uint8_t c);

/** \brief return the last event triggered on the channel defined by uartd

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @return UART_EV_NULL or UART_EV_RX
*/
uint8_t uart_get_event(const uart_descriptor *uartd);

/** \brief reset the last event flag after the event was handled

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
*/
void uart_rst_event(uart_descriptor *uartd);

/** \brief send one byte to the uart channel defined by uartd

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @param byte to be written
*/
void uart_tx(uart_descriptor *uartd, const uint8_t byte);

/** \brief send 'size' number of bytes from the 'str' array to the uart channel defined by uartd

    useful for array of chars that are not null terminated

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @param str array of bytes
    @param size number of bytes to be sent
    @return number of bytes written
*/
uint16_t uart_tx_str(uart_descriptor *uartd, const char *str, const uint16_t size);

/** \brief send a null-terminated string to the uart channel defined by uartd

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @param str array of null-terminated chars
    @return number of bytes written
*/
uint16_t uart_print(uart_descriptor *uartd, const char *str);

#ifdef UART_RX_USES_RINGBUF
/** \brief get pointer to the RX ringbuffer

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @return pointer to the ringbuf struct
*/
struct ringbuf *uart_get_rx_ringbuf(uart_descriptor *uartd);
#define uart_set_eol(a)
#else
/** \brief get pointer to the RX buffer if there is at least one byte in it

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @return pointer to a null-terminated RX buffer
*/
char *uart_get_rx_buf(const uart_descriptor *uartd);

/** \brief get pointer to the RX buffer

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @return pointer to a null-terminated RX buffer
*/
char *uart_get_rx_buf_force(const uart_descriptor *uartd);

/** \brief reset RX buffer counters

    function that needs to be called after the RX buffer was parsed

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
*/
void uart_set_eol(uart_descriptor *uartd);

/** \brief get the number of bytes currently in the input buffer

    @param uartd structure containing the USCI parameters, various counters and the input/output buffers
    @return number of bytes currently in the buffer
*/
uint16_t uart_get_rx_buf_len(uart_descriptor *uartd);
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
///\}
