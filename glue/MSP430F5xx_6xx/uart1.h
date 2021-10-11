#ifndef __UART1_H__
#define __UART1_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(UART1_RX_USES_RINGBUF)
extern struct ringbuf uart1_rbrx;
#endif

extern volatile uint8_t uart1_rx_err;

#define  UART1_EV_NULL 0
#define    UART1_EV_RX 0x1
#define    UART1_EV_TX 0x2

void uart1_init(void);
void uart1_initb(const uint8_t baudrate);
void uart1_port_init(void);

void uart1_tx(const uint8_t byte);
uint16_t uart1_tx_str(const char *str, const uint16_t size);
uint16_t uart1_print(const char *str);

uint8_t uart1_get_event(void);
void uart1_rst_event(void);

void uart1_set_rx_irq_handler(uint8_t (*input)(const uint8_t c));
void uart1_set_tx_irq_handler(uint8_t (*output)(void));

// sample inside-irq handlers
uint8_t uart1_rx_simple_handler(const uint8_t rx);
uint8_t uart1_rx_ringbuf_handler(const uint8_t c);

#ifdef UART1_RX_USES_RINGBUF
struct ringbuf *uart1_get_rx_ringbuf(void);
#define uart1_set_eol()
#else
char *uart1_get_rx_buf(void);
void uart1_set_eol(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
