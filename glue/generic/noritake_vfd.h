/*
* Copyright (c) 2022, Petre Rodan
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer. 
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
* \addtogroup 
* \brief 
* \author Petre Rodan
* 
* \{
**/

#ifndef __NORITAKE_VFD_H_
#define __NORITAKE_VFD_H_

#ifdef CONFIG_VFD_NORITAKE

///< struct who's pointer is passed arround during most calls present in this HAL implementation
// do not read/write to the structure elements directly, try to treat this struct as opaque data
// baseAddress and baudrate are the only variables excepted from the above rule. they need to be 
// set before calling uart_init()
// also if you write your own RX interrupt handler callback all elements can be directly read and written
typedef struct _vfd_descriptor {
    uint8_t last_event;     ///< event flag set during irq handler for main loop 
    uint8_t tx_busy;        ///< true if tx ringbuf is currently sending out data
    struct ringbuf rbtx;     ///< outgoing ring buffer structure
    uint8_t tx_buf[VFD_TXBUF_SZ];  ///< outgoing buffer
#ifdef VFD_USES_SPI
    spi_descriptor *spid;   ///< spi descriptor 
#endif
} vfd_descriptor;



#ifdef __cplusplus
extern "C" {
#endif

void vfd_tx(vfd_descriptor *vfdd, const uint8_t byte);
uint16_t vfd_tx_str(vfd_descriptor *vfdd, const char *str, const uint16_t size);
uint16_t vfd_print(vfd_descriptor *vfdd, const char *str);

void vfd_us_cmd(vfd_descriptor *vfdd, const uint8_t group, const uint8_t cmd);
void vfd_cmd_init(vfd_descriptor *vfdd);
void vfd_cmd_clear(vfd_descriptor *vfdd);

void vfd_invert_off(vfd_descriptor *vfdd);
void vfd_invert_on(vfd_descriptor *vfdd);
void vfd_cmd_xy(vfd_descriptor *vfdd, uint16_t x, uint16_t y);
void vfd_cmd_xy1(vfd_descriptor *vfdd, uint16_t x, uint16_t y);

void vfd_set_font_style(vfd_descriptor *vfdd, const uint8_t proportional, const uint8_t evenSpacing);
void vfd_set_font_size(vfd_descriptor *vfdd, const uint8_t x, const uint8_t y, const uint8_t tall);

#ifdef __cplusplus
}
#endif

#endif

#endif
