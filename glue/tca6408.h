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
* \addtogroup tca6408 8bit i2c IO expander driver
* \brief driver for the TI TCA6408 chip
* \author Petre Rodan
* 
* \{
**/

#ifndef __TCA6408_H__
#define __TCA6408_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

// config.h should contain a define for CONFIG_TCA6408

#define     TCA6408_INPUT  0x0
#define    TCA6408_OUTPUT  0x1
#define   TCA6408_POL_INV  0x2
#define      TCA6408_CONF  0x3

/**
 * \brief start a one byte selective read
 * 
 * this function begins a new I2C multi-byte selective read.
 * 
 * \param usci_base_address MSP430-related register address of the USCI subsystem. 
 *                              can be USCI_B0_BASE - USCI_B1_BASE, EUSCI_B0_BASE - EUSCI_B3_BASE
 * \param slave_addrress    chip i2c address
 * \param data              pointer to pre-allocated buffer where the read data is to be written.
 *                              make sure it's at least data_len in lenght
 * \param addr              FM24 address where the read will start
 **/

uint8_t TCA6408_read(const uint16_t usci_base_addr, const uint8_t slave_addr, uint8_t * data, const uint8_t addr);

/**
 * \brief start a one byte write
 * 
 * this function begins a new I2C multi-byte selective read.
 * 
 * \param usci_base_address MSP430-related register address of the USCI subsystem.
 *                              can be USCI_B0_BASE - USCI_B1_BASE, EUSCI_B0_BASE - EUSCI_B3_BASE
 * \param slave_addrress    chip i2c address
 * \param data              pointer to buffer where the written data is provided
 * \param addr              FM24 address where the write will start
 **/

uint8_t TCA6408_write(const uint16_t usci_base_addr, const uint8_t slave_addr, uint8_t * data, const uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif
