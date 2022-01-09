#ifndef __UART_MAPPING_H__
#define __UART_MAPPING_H__

#define  UART_EV_NULL 0
#define    UART_EV_RX 0x1
#define    UART_EV_TX 0x2

#ifdef UART_USES_UCA0
#define uart_bcl_pin_init()         uart_uca0_pin_init()
#define uart_bcl_init()             uart0_init()
#define uart_bcl_print(a)           uart0_print(a)
#define uart_bcl_tx(a)              uart0_tx(a)
#define uart_bcl_tx_str(a, b)       uart0_tx_str(a, b)
#define uart_bcl_set_eol()          uart0_set_eol()
#define uart_bcl_get_event()        uart0_get_event()
#define uart_bcl_rst_event()        uart0_rst_event()
#define uart_bcl_set_rx_irq_handler(a) uart0_set_rx_irq_handler(a)

#ifdef UART0_RX_USES_RINGBUF
    #define uart_bcl_get_rx_ringbuf()   uart0_get_rx_ringbuf()
    #define uart_bcl_rx_ringbuf_handler uart0_rx_ringbuf_handler
#else
    #define uart_bcl_get_rx_buf()       uart0_get_rx_buf()
    #define uart_bcl_rx_simple_handler  uart0_rx_simple_handler
#endif

#elif UART_USES_UCA1
#define uart_bcl_pin_init()         uart_uca1_pin_init()
#define uart_bcl_init()             uart1_init()
#define uart_bcl_print(a)           uart1_print(a)
#define uart_bcl_tx(a)              uart1_tx(a)
#define uart_bcl_tx_str(a, b)       uart1_tx_str(a, b)
#define uart_bcl_set_eol()          uart1_set_eol()
#define uart_bcl_get_event()        uart1_get_event()
#define uart_bcl_rst_event()        uart1_rst_event()
#define uart_bcl_set_rx_irq_handler(a) uart1_set_rx_irq_handler(a)

#ifdef UART1_RX_USES_RINGBUF
    #define uart_bcl_get_rx_ringbuf()   uart1_get_rx_ringbuf()
    #define uart_bcl_rx_ringbuf_handler uart1_rx_ringbuf_handler
#else
    #define uart_bcl_get_rx_buf()       uart1_get_rx_buf()
    #define uart_bcl_rx_simple_handler  uart1_rx_simple_handler
#endif

#elif UART_USES_UCA2
#define uart_bcl_pin_init()         uart_uca2_pin_init()
#define uart_bcl_init()             uart2_init()
#define uart_bcl_print(a)           uart2_print(a)
#define uart_bcl_tx(a)              uart2_tx(a)
#define uart_bcl_tx_str(a, b)       uart2_tx_str(a, b)
#define uart_bcl_set_eol()          uart2_set_eol()
#define uart_bcl_get_event()        uart2_get_event()
#define uart_bcl_rst_event()        uart2_rst_event()
#define uart_bcl_set_rx_irq_handler(a) uart2_set_rx_irq_handler(a)

#ifdef UART2_RX_USES_RINGBUF
    #define uart_bcl_get_rx_ringbuf()   uart2_get_rx_ringbuf()
    #define uart_bcl_rx_ringbuf_handler uart2_rx_ringbuf_handler
#else
    #define uart_bcl_get_rx_buf()       uart2_get_rx_buf()
    #define uart_bcl_rx_simple_handler  uart2_rx_simple_handler
#endif

#elif UART_USES_UCA3
#define uart_bcl_pin_init()         uart_uca3_pin_init()
#define uart_bcl_init()             uart3_init()
#define uart_bcl_print(a)           uart3_print(a)
#define uart_bcl_tx(a)              uart3_tx(a)
#define uart_bcl_tx_str(a, b)       uart3_tx_str(a, b)
#define uart_bcl_set_eol()          uart3_set_eol()
#define uart_bcl_get_event()        uart3_get_event()
#define uart_bcl_rst_event()        uart3_rst_event()
#define uart_bcl_set_rx_irq_handler(a) uart3_set_rx_irq_handler(a)

#ifdef UART3_RX_USES_RINGBUF
    #define uart_bcl_get_rx_ringbuf()   uart3_get_rx_ringbuf()
    #define uart_bcl_rx_ringbuf_handler uart3_rx_ringbuf_handler
#else
    #define uart_bcl_get_rx_buf()       uart3_get_rx_buf()
    #define uart_bcl_rx_simple_handler  uart3_rx_simple_handler
#endif

#else
#error no UART_USES_UCAx option was defined in config.h
#endif

#endif
