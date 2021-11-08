#ifndef __UART3_H__
#define __UART3_H__

#ifdef UART3_TX_USES_RINGBUF
#ifndef UART3_TX_USES_IRQ
#define UART3_TX_USES_IRQ
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(UART3_RX_USES_RINGBUF)
extern struct ringbuf uart3_rbrx;
#endif

extern volatile uint8_t uart3_rx_err;

#define  UART3_EV_NULL 0
#define    UART3_EV_RX 0x1
#define    UART3_EV_TX 0x2

void uart3_init(void);
void uart3_initb(const uint8_t baudrate);
void uart3_port_init(void);

void uart3_tx(const uint8_t byte);
uint16_t uart3_tx_str(const char *str, const uint16_t size);
uint16_t uart3_print(const char *str);

uint8_t uart3_get_event(void);
void uart3_rst_event(void);

void uart3_set_rx_irq_handler(uint8_t (*input)(const uint8_t c));
void uart3_set_tx_irq_handler(uint8_t (*output)(void));

// sample inside-irq handlers
uint8_t uart3_rx_simple_handler(const uint8_t rx);
uint8_t uart3_rx_ringbuf_handler(const uint8_t c);

#ifdef UART3_RX_USES_RINGBUF
struct ringbuf *uart3_get_rx_ringbuf(void);
#define uart3_set_eol()
#else
char *uart3_get_rx_buf(void);
void uart3_set_eol(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
