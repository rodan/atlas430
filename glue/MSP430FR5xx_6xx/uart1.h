#ifndef __UART1_H__
#define __UART1_H__

#ifdef __cplusplus
extern "C" {
#endif

#define  UART1_EV_NULL 0
#define    UART1_EV_RX 0x1
#define    UART1_EV_TX 0x2

#define UART1_RXBUF_SZ 64

void uart1_init();
void uart1_port_init(void);
uint16_t uart1_tx_str(const char *str, const uint16_t size);
uint16_t uart1_print(const char *str);
uint8_t uart1_get_event(void);
void uart1_rst_event(void);
void uart1_set_eol(void);
char *uart1_get_rx_buf(void);

#ifdef __cplusplus
}
#endif

#endif
