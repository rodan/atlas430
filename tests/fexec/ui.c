
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "version.h"
#include "ui.h"

extern uart_descriptor bc;

static const char menu_str[]="\
 available commands:\r\n\r\n\
\e[33;1m?\e[0m  - show menu\r\n\
\e[33;1mt\e[0m  - test fct\r\n";

#define STR_LEN 80

// execute internal function
#define SCENARIO1

// execute detected opcodes without mmap, also not obeying NX
#define SCENARIO5

#define OPCODE_LEN 8
uint8_t f_opcode[OPCODE_LEN] = {0x03, 0x43, 0x7c, 0x40, 0xee, 0xff, 0x30, 0x41};

uint8_t f_int(void)
{
    __asm("nop");
    return 0xee;
}

void display_menu(void)
{
    display_version();
    uart_print(&bc, menu_str);
}

void display_version(void)
{
    char sconv[CONV_BASE_10_BUF_SZ];

    uart_print(&bc, "fexec b");
    uart_print(&bc, _utoa(sconv, BUILD));
    uart_print(&bc, "\r\n");
}

// same as above, but asume writable memory can be executed and no mmap is needed
uint8_t exec_opcode_no_mmap(const uint8_t *code, size_t codelen)
{
    uint8_t ret;

    // now we can call it
    ret = ((uint8_t (*)(void)) code)();
    return ret;
}

#define PARSER_CNT 16

void parse_user_input(void)
{
#ifdef UART_RX_USES_RINGBUF
    struct ringbuf *rbr = uart_get_rx_ringbuf(&bc);
    uint8_t rx;
    uint8_t c = 0;
    char input[PARSER_CNT];

    memset(input, 0, PARSER_CNT);

    // read the entire ringbuffer
    while (ringbuf_get(rbr, &rx)) {
        if (c < PARSER_CNT-1) {
            input[c] = rx;
        }
        c++;
    }
#else
    char *input = uart_get_rx_buf(&bc);
#endif
    char f = input[0];
    char itoa_buf[CONV_BASE_10_BUF_SZ];
    uint8_t current_opcode[OPCODE_LEN];
    uint8_t ret;
    char buf[STR_LEN+1];
    uint8_t i;
    uint8_t *ap;

    if (f == '?') {
        display_menu();
    } else if (f == 't') {
        ap = (uint8_t *)&f_int;

        memcpy(current_opcode, ap, OPCODE_LEN);
        if (memcmp(current_opcode, f_opcode, OPCODE_LEN) != 0) {

            uart_print(&bc, " [!!] opcodes should be updated\r\n");
            for (i=0;i<8;i++) {
                uart_print(&bc, _utoh8(itoa_buf,(uint8_t)(*(ap+4*i))));
                uart_print(&bc, _utoh8(itoa_buf,(uint8_t)(*(ap+4*i+1))));
                uart_print(&bc, _utoh8(itoa_buf,(uint8_t)(*(ap+4*i+2))));
                uart_print(&bc, _utoh8(itoa_buf,(uint8_t)(*(ap+4*i+3))));
                uart_print(&bc, " ");
            }
            uart_print(&bc, "\r\nuint8_t f_opcode[");
            uart_print(&bc, _utoa(itoa_buf,OPCODE_LEN));
            uart_print(&bc, "] = {");

            for (i=0; i<OPCODE_LEN; i++) {
                uart_print(&bc, _utoh(itoa_buf,(uint8_t)(*(ap+i))));
                if (i < (OPCODE_LEN-1)) {
                    uart_print(&bc, ", ");
                }
            }
            uart_print(&bc, "};\r\n");
        }

#ifdef SCENARIO1
        uart_print(&bc, "* execute internal function ");
        ret = f_int();
        if (ret == 0xee) {
            uart_print(&bc, " \t[ok]\r\n");
        } else {
            uart_print(&bc, " \t[failed]\r\n");
            uart_print(&bc, buf);
        }
#endif

#ifdef SCENARIO5
        uart_print(&bc, "* execute detected opcodes ");
        ret = exec_opcode_no_mmap(current_opcode, sizeof(f_opcode));
        if (ret == 0xee) {
            uart_print(&bc, " \t[ok]\r\n");
        } else {
            uart_print(&bc, " \t[failed]\r\n");
        }
#endif
    }
}

