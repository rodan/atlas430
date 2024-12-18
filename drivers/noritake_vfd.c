
#include "warning.h"

#ifdef CONFIG_VFD_NORITAKE

#include <stdint.h>
#include <string.h>
#include "atlas430.h"
#include "sig.h"

static void command(vfd_descriptor *vfdd, uint8_t data)
{
#ifdef VFD_USES_SPI
    vfdd->spid->cs_low();
    spi_write_frame(vfdd->spid->baseAddress, &data, 1);
    vfdd->spid->cs_high();
#endif
}

static void vfd_tx_activate(vfd_descriptor *vfdd)
{
    uint8_t t;
            
    sig5_on;

    if (!vfdd->tx_busy) {
        if (ringbuf_get(&vfdd->rbtx, &t)) {
            vfdd->tx_busy = 1;
            command(vfdd, t);
        } else {
            // nothing more to do
            vfdd->tx_busy = 0;
        }
    }
    sig5_off;
}

void vfd_us_cmd(vfd_descriptor *vfdd, const uint8_t group, const uint8_t cmd)
{
    uint8_t data[4];

    data[0] = 0x1f;
    data[1] = 0x28;
    data[2] = group;
    data[3] = cmd;

    vfd_tx_str(vfdd, (char *) data, 4);
}

void vfd_invert_off(vfd_descriptor *vfdd) {
    uint8_t data[3] = {0x1f, 'r', 0};

    vfd_tx_str(vfdd, (char *) data, 3);
}

void vfd_invert_on(vfd_descriptor *vfdd) {
    uint8_t data[3] = {0x1f, 'r', 1};

    vfd_tx_str(vfdd, (char *) data, 3);
}

void vfd_cmd_xy(vfd_descriptor *vfdd, uint16_t x, uint16_t y) {
    uint8_t data[4];

    data[0] = x & 0xff;
    data[1] = x >> 8;
    y /= 8;
    data[2] = y & 0xff;
    data[3] = y >> 8;

    vfd_tx_str(vfdd, (char *) data, 4);
}

void vfd_cmd_xy1(vfd_descriptor *vfdd, uint16_t x, uint16_t y) {
    uint8_t data[4];

    data[0] = x & 0xff;
    data[1] = x >> 8;
    data[2] = y & 0xff;
    data[3] = y >> 8;

    vfd_tx_str(vfdd, (char *) data, 4);
}

void vfd_set_font_style(vfd_descriptor *vfdd, const uint8_t proportional, const uint8_t evenSpacing) {
    uint8_t data;
    
    vfd_us_cmd(vfdd, 'g', 0x03);
    data = proportional*2 + evenSpacing;
    vfd_tx(vfdd, data);
}

void vfd_set_font_size(vfd_descriptor *vfdd, const uint8_t x, const uint8_t y, const uint8_t tall) {
    uint8_t data[2];

    if (x<=4 && y<=2) {
        vfd_us_cmd(vfdd, 'g', 0x40);
        data[0] = x;
        data[1] = y;

        vfd_tx_str(vfdd, (char *) data, 2);
#if 0
        #if (NORITAKE_VFD_MODEL_CLASS-7000)/100==9 || (NORITAKE_VFD_MODEL_CLASS-7000)/100==1
            us_command('g', 0x01);
            command(tall+1);
        #endif
#endif
    }
}

void vfd_cmd_init(vfd_descriptor *vfdd)
{
    uint8_t data[2] = { 0x1b, '@'};

    vfd_tx_str(vfdd, (char *) data, 2);
}

void vfd_cmd_clear(vfd_descriptor *vfdd)
{
    vfd_tx(vfdd, 0x0c);
}

void vfd_tx(vfd_descriptor *vfdd, const uint8_t data)
{
    while (ringbuf_put(&vfdd->rbtx, data) == 0) {
        // wait for the ring buffer to clear
        vfd_tx_activate(vfdd);
        _BIS_SR(LPM0_bits + GIE);
    }

    vfd_tx_activate(vfdd);
}

uint16_t vfd_tx_str(vfd_descriptor *vfdd, const char *str, const uint16_t size)
{
    uint16_t p = 0;

    while (p < size) {        
        if (ringbuf_put(&vfdd->rbtx, str[p])) {
            p++;
            vfd_tx_activate(vfdd);
        } else {
            _BIS_SR(LPM0_bits + GIE);
        }
    }
    return p;
}

uint16_t vfd_print(vfd_descriptor *vfdd, const char *str)
{
    size_t p = 0;
    size_t size = strlen(str);
    while (p < size) {
        if (ringbuf_put(&vfdd->rbtx, str[p])) {
            p++;
            vfd_tx_activate(vfdd);
        } else {
            _BIS_SR(LPM0_bits + GIE);
        }
    }
    return p;
}

#endif
