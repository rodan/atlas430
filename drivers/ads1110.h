/*
* Copyright (c) 2020, Petre Rodan
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
* \addtogroup ads1110 Burr-Brown 16 bit ADC with local reference
* \brief driver for the BB ADS1110 chip
* \author Petre Rodan
* 
* \{
**/


#ifndef __ADS1110_H__
#define __ADS1110_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

// you must define the slave address.
// it can be determined based on the external marking
#define           ADS1110_ED0  0x48
#define           ADS1110_ED1  0x49
#define           ADS1110_ED2  0x4a
#define           ADS1110_ED3  0x4b
#define           ADS1110_ED4  0x4c
#define           ADS1110_ED5  0x4d
#define           ADS1110_ED6  0x4e
#define           ADS1110_ED7  0x4f

// configuration register bits
#define         ADS1110_PGA_1  0x0
#define         ADS1110_PGA_2  0x1
#define         ADS1110_PGA_4  0x2
#define         ADS1110_PGA_8  0x3
#define       ADS1110_BITS_12  0x0
#define       ADS1110_BITS_14  0x4
#define       ADS1110_BITS_15  0x8
#define       ADS1110_BITS_16  0xc
#define            ADS1110_SC  0x10
#define            ADS1110_ST  0x80

// if the chip only does a single conversion, the state machine can reach these
#define ADS1110_STATE_STANDBY  0x0
#define ADS1110_STATE_CONVERT  0x1

struct ads1110_t {
    int16_t conv_raw;   // adc raw output
    int16_t conv;       // int voltage value *10^4
    uint8_t config;     // adc configuration register
    uint8_t state;      // state machine parameter
};

uint8_t ADS1110_read(const uint16_t usci_base_addr, const uint8_t slave_addr, struct ads1110_t *adc);
void ADS1110_convert(struct ads1110_t *adc);
uint8_t ADS1110_config(const uint16_t usci_base_addr, const uint8_t slave_addr, const uint8_t val);
uint8_t ADS1110_init(const uint16_t usci_base_addr, const uint8_t slave_address, const uint8_t config_reg);

#ifdef __cplusplus
}
#endif

#endif

