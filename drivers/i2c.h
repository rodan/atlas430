/*
* Copyright (c) 2013, Alexander I. Mykyta
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
* \addtogroup MOD_I2C I2C Bus
* \brief Application-level I2C master driver.
* \author Alex Mykyta 
*
* 
* ### MSP430 Processor Families Supported: ###
*   Family  | Supported
*   ------- | ----------
*   1xx     | -
*   2xx     | -
*   4xx     | -
*   5xx     | Yes
*   6xx     | Yes
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_I2C
* \author Alex Mykyta 
**/

#ifndef _I2C_H_
#define _I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// option flags
#define I2C_READ                0x1
#define I2C_WRITE               0x2
// sometimes the master has to wait for the slave to release SDA
// some sensors pull SDA low to signal that data is ready
#define I2C_SDA_WAIT            0x4
// some devices want the last read byte to not be ACKed
#define I2C_LAST_NAK            0x8
#define I2C_NO_ADDR_SHIFT       0x10
#define I2C_REPEAT_SA_ON_READ   0x20
#define I2C_SHT_INIT            0x40

typedef struct {
    uint8_t slave_addr;     ///< Chip address of slave device
    uint8_t *addr;          ///< register/command payload
    uint16_t addr_len;      ///< Number of addr bytes to use (1-3)
    uint8_t *data;          ///< Pointer to data transfer buffer
    uint16_t data_len;      ///< Number of bytes to transfer
    uint8_t options;        ///< see above the possible option flags
} i2c_package_t;

i2c_package_t pkg;

typedef enum {
    I2C_IDLE,               ///< Bus is idle. Ready for new transfer.
    I2C_BUSY,               ///< A transfer is in progress.
    I2C_FAILED              ///< Previous transfer failed. Ready for new transfer.
} i2c_status_t;

/**
 * \brief Initialize the I2C master module
 **/
    void i2c_init(void);

/**
 * \brief Uninitialize the I2C master module
 **/
    void i2c_uninit(void);

/**
 * \brief Start an I2C transfer
 * 
 * This function begins a new I2C transaction as described by the \c pkg struct. This function
 * is nonblocking and returns immediately after the transfer is started. The status of the transfer 
 * can be polled using the i2c_transfer_status() function. Alternatively, a \c callback function can
 * be executed when the transfer completes.
 * 
 * \note Global interrupts must be enabled.
 * 
 * \param pkg        Pointer to a package struct that describes the transfer operation.
 * \param callback   Optional pointer to a callback function to execute once the transfer completes
 *                   or fails. A NULL pointer disables the callback.
 **/
    void i2c_transfer_start(const i2c_package_t * pkg,
                            void (*callback) (i2c_status_t result));

/**
 * \brief Get the status of the I2C module
 * \return status of the bus.
 **/
    i2c_status_t i2c_transfer_status(void);

#ifdef __cplusplus
}
#endif
#endif
///\}
