
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "ui.h"

static const char menu_str[]="\r\n fexec test module --- available commands:\r\n\r\n\
\e[33;1m?\e[0m  - show menu\r\n\
\e[33;1mr\e[0m  - run fct\r\n";

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
    uart0_print(menu_str);
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
#ifdef UART0_RX_USES_RINGBUF
    struct ringbuf *rbr = uart0_get_rx_ringbuf();
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
    char *input = uart0_get_rx_buf();
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
    } else if (f == 'r') {
        ap = (uint8_t *)&f_int;

        memcpy(current_opcode, ap, OPCODE_LEN);
        if (memcmp(current_opcode, f_opcode, OPCODE_LEN) != 0) {

            uart0_print(" [!!] opcodes should be updated\r\n");
            for (i=0;i<8;i++) {
                uart0_print(_utoh8(itoa_buf,(uint8_t)(*(ap+4*i))));
                uart0_print(_utoh8(itoa_buf,(uint8_t)(*(ap+4*i+1))));
                uart0_print(_utoh8(itoa_buf,(uint8_t)(*(ap+4*i+2))));
                uart0_print(_utoh8(itoa_buf,(uint8_t)(*(ap+4*i+3))));
                uart0_print(" ");
            }
            uart0_print("\r\nuint8_t f_opcode[");
            uart0_print(_utoa(itoa_buf,OPCODE_LEN));
            uart0_print("] = {");

            for (i=0; i<OPCODE_LEN; i++) {
                uart0_print(_utoh(itoa_buf,(uint8_t)(*(ap+i))));
                if (i < (OPCODE_LEN-1)) {
                    uart0_print(", ");
                }
            }
            uart0_print("};\r\n");
        }

#ifdef SCENARIO1
        uart0_print("* execute internal function ");
        ret = f_int();
        if (ret == 0xee) {
            uart0_print(" \t[ok]\r\n");
        } else {
            uart0_print(" \t[failed]\r\n");
            uart0_print(buf);
        }
#endif

#ifdef SCENARIO5
        uart0_print("* execute detected opcodes ");
        ret = exec_opcode_no_mmap(current_opcode, sizeof(f_opcode));
        if (ret == 0xee) {
            uart0_print(" \t[ok]\r\n");
        } else {
            uart0_print(" \t[failed]\r\n");
        }
#endif
    }
}

