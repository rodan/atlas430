#ifndef __PGA2311_H__
#define __PGA2311_H__

/**
* \addtogroup MOD_DRIVERS Texas Instruments PGA2311 IC driver
* \brief driver for the TI PGA2311 stereo audio volume control chip
    \note the CS signal is managed by the library, but make sure the spi_descriptor is populated correctly
* \author Petre Rodan
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_DRIVERS
* \author Petre Rodan
**/

/** set the volume for both channels of the PGA
    @param spid         descriptor containing the SPI subsystem base address and CS signal control
    @param vol_right    potentiometer tap for right channel [0-255]
    @param vol_left     potentiometer tap for left channel [0-255]
*/
void pga_set_volume(const spi_descriptor *spid, const uint8_t vol_right,
                const uint8_t vol_left);

#endif
///\}
