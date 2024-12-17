// C library for the MCP42010, MCP42050, MCP42100 digital potentiometer ICs

//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/
//   license:         GNU GPLv3

#include <inttypes.h>
#include "spi.h"
#include "mcp42.h"

void MCP42_set_pot_ch(const spi_descriptor *spid, const uint8_t pot_id,
                const uint8_t pot_val)
{
    uint8_t data[2];

    // data[0] is the command byte, data[1] is the data byte
    data[0] = MCP42_WRITE_DATA | ( pot_id + 1 );
    data[1] = pot_val;

    spid->cs_low();
    spi_write_frame(spid->baseAddress, data, 2);
    spid->cs_high();
}

void MCP42_set_pot(const spi_descriptor *spid, const uint8_t pot_ch0_val, const uint8_t pot_ch1_val)
{
    uint8_t data[4];

    // data[0] is the command byte, data[1] is the data byte
    data[0] = MCP42_WRITE_DATA | 1;
    data[1] = pot_ch0_val;
    data[2] = MCP42_WRITE_DATA | 2;
    data[3] = pot_ch1_val;

    spid->cs_low();
    spi_write_frame(spid->baseAddress, data, 4);
    spid->cs_high();
}

void MCP42_shutdown_pot(const spi_descriptor *spid, const uint8_t pot_id)
{
    uint8_t data[2];

    // data[0] is the command byte, data[1] is the data byte
    data[0] = MCP42_SHUTDOWN | ( pot_id + 1 );
    data[1] = 0;

    spid->cs_low();
    spi_write_frame(spid->baseAddress, data, 2);
    spid->cs_high();
}

