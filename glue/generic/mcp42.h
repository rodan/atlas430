#ifndef __MCP42XXX_H__
#define __MCP42XXX_H__

/**
* \addtogroup MOD_DRIVERS MCP42xxx IC driver
* \brief driver for the MCP42xxx dual digital potentiometer chip
    \note the CS signal is managed by the library, but make sure the spi_descriptor is populated correctly
    \note during write transfers, there is no need to set the most significat bit in the address byte to one, the library takes care of it
* \author Petre Rodan
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_DRIVERS
* \author Petre Rodan
**/


//! MCP42XXX bitflags
#define MCP42_WRITE_DATA   0x10
#define MCP42_SHUTDOWN     0x20

#ifdef __cplusplus
extern "C" {
#endif

#include <msp430.h>

/** set the potentiometer value to one of the channels of the chip
    @param spid     descriptor containing the SPI subsystem base address and CS signal control
    @param pot_id   the channel/pot from within the chip [0-1]
    @param pot_val  potentiometer value [0-255]
*/
void MCP42_set_pot_ch(const spi_descriptor *spid, const uint8_t pot_id,
                const uint8_t pot_val);

/** set the potentiometer values of both channels of the chip
    @param spid         descriptor containing the SPI subsystem base address and CS signal control
    @param pot_ch0_val  potentiometer value for the first channel [0-255]
    @param pot_ch1_val  potentiometer value for the second channel [0-255]
*/
void MCP42_set_pot(const spi_descriptor *spid, const uint8_t pot_ch0_val, const uint8_t pot_ch1_val);


/** shutdown one of the channels of the chip identified by 'mcp_id'
    @param spid     descriptor containing the SPI subsystem base address and CS signal control
    @param pot_id   the channel/pot from within the chip [0-1]
*/
void MCP42_shutdown_pot(const spi_descriptor *spid, const uint8_t pot_id);

#ifdef __cplusplus
}
#endif

#endif

///\}
