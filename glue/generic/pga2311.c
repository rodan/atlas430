
//
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/ampy
//  license:         GNU GPLv3

#include <stdint.h>
#include "spi.h"
#include "pga2311.h"

void pga_set_volume(const spi_descriptor *spid, const uint8_t vol_right,
                const uint8_t vol_left)
{
    uint8_t data[2];

    data[0] = vol_right;
    data[1] = vol_left;

    spid->cs_low();
    spi_write_frame(spid->baseAddress, data, 2);
    spid->cs_high();
}

