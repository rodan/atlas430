
#include <stdint.h>
#include "exf.h"

#define  EXF_PAGE_START  0x0
#define    EXF_PAGE_END  0x43f
#define   EXF_PAGE_SIZE  0x100

void exf_send(const uint8_t byte);

void exf(void)
{
    uint32_t page;
    uint32_t addr;
    uint8_t *p;

    cs_high;
    clk_low;

    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;

    for (page = EXF_PAGE_START; page <= EXF_PAGE_END; page++) {
        cs_low;
        exf_send((page * EXF_PAGE_SIZE) >> 8);
        exf_send((page * EXF_PAGE_SIZE) & 0xff);
        cs_high;

        cs_low;
        for (addr = page * EXF_PAGE_SIZE; addr < (page + 1) * EXF_PAGE_SIZE; addr++) {
            // skip some pages
            if ((addr > 0x1aff && addr < 0x1c00) ||
                (addr > 0x1000 && addr < 0x19ff) || (addr > 0x500 && addr < 0x5ff)) {
                exf_send(0xab);
            } else {
                p = (uint8_t *) (addr);
                exf_send(*p);
            }
        }
        cs_high;
    }
}

void exf_send(const uint8_t byte)
{
    int8_t i;

    for (i = 8; i > 0; i--) {
        if (byte & (1 << (i - 1))) {
            mosi_high;
        } else {
            mosi_low;
        }

        clk_high;
        clk_low;
    }
}
