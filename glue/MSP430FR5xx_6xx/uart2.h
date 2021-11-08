#ifndef __UART2_H__
#define __UART2_H__

#ifdef UART2_TX_USES_RINGBUF
#ifndef UART2_TX_USES_IRQ
#define UART2_TX_USES_IRQ
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(UART2_RX_USES_RINGBUF)
extern struct ringbuf uart2_rbrx;
#endif

extern volatile uint8_t uart2_rx_err;

#define  UART2_EV_NULL 0
#define    UART2_EV_RX 0x1
#define    UART2_EV_TX 0x2

void uart2_init(void);
void uart2_initb(const uint8_t baudrate);
void uart2_port_init(void);

void uart2_tx(const uint8_t byte);
uint16_t uart2_tx_str(const char *str, const uint16_t size);
uint16_t uart2_print(const char *str);

uint8_t uart2_get_event(void);
void uart2_rst_event(void);

void uart2_set_rx_irq_handler(uint8_t (*input)(const uint8_t c));
void uart2_set_tx_irq_handler(uint8_t (*output)(void));

// sample inside-irq handlers
uint8_t uart2_rx_simple_handler(const uint8_t rx);
uint8_t uart2_rx_ringbuf_handler(const uint8_t c);

#ifdef UART2_RX_USES_RINGBUF
struct ringbuf *uart2_get_rx_ringbuf(void);
#define uart2_set_eol()
#else
char *uart2_get_rx_buf(void);
void uart2_set_eol(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
