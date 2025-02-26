/*
* Copyright (c) 2024-2025, Petre Rodan
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

#ifndef __HBMPS_H_
#define __HBMPS_H_

#include <inttypes.h>

#define      HBMPS_BUFF_SIZE  4
#define             HBMPS_OK  0x0
#define HBMPS_ERR_DIAG_FAULT  0x0300

struct hbmps_pkt {
    uint8_t status;             // 2 bit
    uint16_t bridge_data;       // 14 bit
    uint16_t temperature_data;  // 11 bit
};

 /**
  * consult the datasheet nomenclature to see which transfer function your chip uses
  *
  * function A: 10% - 90% of 2^14
  * function B:  5% - 95% of 2^14
  * function C:  5% - 85% of 2^14
  * function F:  4% - 94% of 2^14
  * 
  * based on the transfer function hbmps_spec.output_min, 
  *   hbmps_spec.output_max will take these values
  *
  * [HSC_FUNCTION_A] = { .output_min = 1638, .output_max = 14746 },
  * [HSC_FUNCTION_B] = { .output_min =  819, .output_max = 15565 },
  * [HSC_FUNCTION_C] = { .output_min =  819, .output_max = 13926 },
  * [HSC_FUNCTION_F] = { .output_min =  655, .output_max = 15401 },
  * 
  * based on the product id triplet, hbmps_spec.pressure_min and
  * pressure_max have to be selected from the table below
  *
  * [HSC001BA] = { .pmin =       0, .pmax =  100000 },
  * [HSC1_6BA] = { .pmin =       0, .pmax =  160000 },
  * [HSC2_5BA] = { .pmin =       0, .pmax =  250000 },
  * [HSC004BA] = { .pmin =       0, .pmax =  400000 },
  * [HSC006BA] = { .pmin =       0, .pmax =  600000 },
  * [HSC010BA] = { .pmin =       0, .pmax = 1000000 },
  * [HSC1_6MD] = { .pmin =    -160, .pmax =     160 },
  * [HSC2_5MD] = { .pmin =    -250, .pmax =     250 },
  * [HSC004MD] = { .pmin =    -400, .pmax =     400 },
  * [HSC006MD] = { .pmin =    -600, .pmax =     600 },
  * [HSC010MD] = { .pmin =   -1000, .pmax =    1000 },
  * [HSC016MD] = { .pmin =   -1600, .pmax =    1600 },
  * [HSC025MD] = { .pmin =   -2500, .pmax =    2500 },
  * [HSC040MD] = { .pmin =   -4000, .pmax =    4000 },
  * [HSC060MD] = { .pmin =   -6000, .pmax =    6000 },
  * [HSC100MD] = { .pmin =  -10000, .pmax =   10000 },
  * [HSC160MD] = { .pmin =  -16000, .pmax =   16000 },
  * [HSC250MD] = { .pmin =  -25000, .pmax =   25000 },
  * [HSC400MD] = { .pmin =  -40000, .pmax =   40000 },
  * [HSC600MD] = { .pmin =  -60000, .pmax =   60000 },
  * [HSC001BD] = { .pmin = -100000, .pmax =  100000 },
  * [HSC1_6BD] = { .pmin = -160000, .pmax =  160000 },
  * [HSC2_5BD] = { .pmin = -250000, .pmax =  250000 },
  * [HSC004BD] = { .pmin = -400000, .pmax =  400000 },
  * [HSC2_5MG] = { .pmin =       0, .pmax =     250 },
  * [HSC004MG] = { .pmin =       0, .pmax =     400 },
  * [HSC006MG] = { .pmin =       0, .pmax =     600 },
  * [HSC010MG] = { .pmin =       0, .pmax =    1000 },
  * [HSC016MG] = { .pmin =       0, .pmax =    1600 },
  * [HSC025MG] = { .pmin =       0, .pmax =    2500 },
  * [HSC040MG] = { .pmin =       0, .pmax =    4000 },
  * [HSC060MG] = { .pmin =       0, .pmax =    6000 },
  * [HSC100MG] = { .pmin =       0, .pmax =   10000 },
  * [HSC160MG] = { .pmin =       0, .pmax =   16000 },
  * [HSC250MG] = { .pmin =       0, .pmax =   25000 },
  * [HSC400MG] = { .pmin =       0, .pmax =   40000 },
  * [HSC600MG] = { .pmin =       0, .pmax =   60000 },
  * [HSC001BG] = { .pmin =       0, .pmax =  100000 },
  * [HSC1_6BG] = { .pmin =       0, .pmax =  160000 },
  * [HSC2_5BG] = { .pmin =       0, .pmax =  250000 },
  * [HSC004BG] = { .pmin =       0, .pmax =  400000 },
  * [HSC006BG] = { .pmin =       0, .pmax =  600000 },
  * [HSC010BG] = { .pmin =       0, .pmax = 1000000 },
  * [HSC100KA] = { .pmin =       0, .pmax =  100000 },
  * [HSC160KA] = { .pmin =       0, .pmax =  160000 },
  * [HSC250KA] = { .pmin =       0, .pmax =  250000 },
  * [HSC400KA] = { .pmin =       0, .pmax =  400000 },
  * [HSC600KA] = { .pmin =       0, .pmax =  600000 },
  * [HSC001GA] = { .pmin =       0, .pmax = 1000000 },
  * [HSC160LD] = { .pmin =    -160, .pmax =     160 },
  * [HSC250LD] = { .pmin =    -250, .pmax =     250 },
  * [HSC400LD] = { .pmin =    -400, .pmax =     400 },
  * [HSC600LD] = { .pmin =    -600, .pmax =     600 },
  * [HSC001KD] = { .pmin =   -1000, .pmax =    1000 },
  * [HSC1_6KD] = { .pmin =   -1600, .pmax =    1600 },
  * [HSC2_5KD] = { .pmin =   -2500, .pmax =    2500 },
  * [HSC004KD] = { .pmin =   -4000, .pmax =    4000 },
  * [HSC006KD] = { .pmin =   -6000, .pmax =    6000 },
  * [HSC010KD] = { .pmin =  -10000, .pmax =   10000 },
  * [HSC016KD] = { .pmin =  -16000, .pmax =   16000 },
  * [HSC025KD] = { .pmin =  -25000, .pmax =   25000 },
  * [HSC040KD] = { .pmin =  -40000, .pmax =   40000 },
  * [HSC060KD] = { .pmin =  -60000, .pmax =   60000 },
  * [HSC100KD] = { .pmin = -100000, .pmax =  100000 },
  * [HSC160KD] = { .pmin = -160000, .pmax =  160000 },
  * [HSC250KD] = { .pmin = -250000, .pmax =  250000 },
  * [HSC400KD] = { .pmin = -400000, .pmax =  400000 },
  * [HSC250LG] = { .pmin =       0, .pmax =     250 },
  * [HSC400LG] = { .pmin =       0, .pmax =     400 },
  * [HSC600LG] = { .pmin =       0, .pmax =     600 },
  * [HSC001KG] = { .pmin =       0, .pmax =    1000 },
  * [HSC1_6KG] = { .pmin =       0, .pmax =    1600 },
  * [HSC2_5KG] = { .pmin =       0, .pmax =    2500 },
  * [HSC004KG] = { .pmin =       0, .pmax =    4000 },
  * [HSC006KG] = { .pmin =       0, .pmax =    6000 },
  * [HSC010KG] = { .pmin =       0, .pmax =   10000 },
  * [HSC016KG] = { .pmin =       0, .pmax =   16000 },
  * [HSC025KG] = { .pmin =       0, .pmax =   25000 },
  * [HSC040KG] = { .pmin =       0, .pmax =   40000 },
  * [HSC060KG] = { .pmin =       0, .pmax =   60000 },
  * [HSC100KG] = { .pmin =       0, .pmax =  100000 },
  * [HSC160KG] = { .pmin =       0, .pmax =  160000 },
  * [HSC250KG] = { .pmin =       0, .pmax =  250000 },
  * [HSC400KG] = { .pmin =       0, .pmax =  400000 },
  * [HSC600KG] = { .pmin =       0, .pmax =  600000 },
  * [HSC001GG] = { .pmin =       0, .pmax = 1000000 },
  * [HSC015PA] = { .pmin =       0, .pmax =  103421 },
  * [HSC030PA] = { .pmin =       0, .pmax =  206843 },
  * [HSC060PA] = { .pmin =       0, .pmax =  413685 },
  * [HSC100PA] = { .pmin =       0, .pmax =  689476 },
  * [HSC150PA] = { .pmin =       0, .pmax = 1034214 },
  * [HSC0_5ND] = { .pmin =    -125, .pmax =     125 },
  * [HSC001ND] = { .pmin =    -249, .pmax =     249 },
  * [HSC002ND] = { .pmin =    -498, .pmax =     498 },
  * [HSC004ND] = { .pmin =    -996, .pmax =     996 },
  * [HSC005ND] = { .pmin =   -1245, .pmax =    1245 },
  * [HSC010ND] = { .pmin =   -2491, .pmax =    2491 },
  * [HSC020ND] = { .pmin =   -4982, .pmax =    4982 },
  * [HSC030ND] = { .pmin =   -7473, .pmax =    7473 },
  * [HSC001PD] = { .pmin =   -6895, .pmax =    6895 },
  * [HSC005PD] = { .pmin =  -34474, .pmax =   34474 },
  * [HSC015PD] = { .pmin = -103421, .pmax =  103421 },
  * [HSC030PD] = { .pmin = -206843, .pmax =  206843 },
  * [HSC060PD] = { .pmin = -413685, .pmax =  413685 },
  * [HSC001NG] = { .pmin =       0, .pmax =     249 },
  * [HSC002NG] = { .pmin =       0, .pmax =     498 },
  * [HSC004NG] = { .pmin =       0, .pmax =     996 },
  * [HSC005NG] = { .pmin =       0, .pmax =    1245 },
  * [HSC010NG] = { .pmin =       0, .pmax =    2491 },
  * [HSC020NG] = { .pmin =       0, .pmax =    4982 },
  * [HSC030NG] = { .pmin =       0, .pmax =    7473 },
  * [HSC001PG] = { .pmin =       0, .pmax =    6895 },
  * [HSC005PG] = { .pmin =       0, .pmax =   34474 },
  * [HSC015PG] = { .pmin =       0, .pmax =  103421 },
  * [HSC030PG] = { .pmin =       0, .pmax =  206843 },
  * [HSC060PG] = { .pmin =       0, .pmax =  413685 },
  * [HSC100PG] = { .pmin =       0, .pmax =  689476 },
  * [HSC150PG] = { .pmin =       0, .pmax = 1034214 },
  *
  */

typedef struct hbmps_spec {
    uint16_t output_min;
    uint16_t output_max;
    int32_t pressure_min;
    int32_t pressure_max;
} hbmps_spec_t;

 /** for i2c-based chips the slave address can be extracted from the part number:
  *    _SC_________XA_
  *    where X can be one of:
  *
  *    S  - spi
  *    2  - i2c slave address 0x28
  *    3  - i2c slave address 0x38
  *    4  - i2c slave address 0x48
  *    5  - i2c slave address 0x58
  *    6  - i2c slave address 0x68
  *    7  - i2c slave address 0x78
*/

#ifdef __cplusplus
extern "C" {
#endif

/** read all 4 registers of a Honeywell pressure sensor IC
    @param dev     device pointer
    @param raw_pkt pre-allocated struct that holds the readout data
    @return non-zero on failure
*/
uint16_t hbmps_read(device_t *dev, struct hbmps_pkt *raw_pkt);

/** calculate the pressure and temperature contained 
    @param raw_pkt      pre-allocated struct that holds the readout data
    @param pressure     pre-allocated integer that will contain the calculated pressure (measured in pascals)
    @param temperature  pre-allocated integer that will contain the calculated temperature (measured in degrees C, multiplied by 100)
    @param spec         pointer to struct containing the specifications of the current sensor - based on part number nomenclature
    @return non-zero on failure
*/
uint16_t hbmps_convert(const struct hbmps_pkt *raw_pkt, int32_t *pressure,
                   int32_t *temperature, const hbmps_spec_t *spec);

#ifdef __cplusplus
}
#endif

#endif
