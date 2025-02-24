#ifndef __SERIAL_BITBANG_
#define __SERIAL_BITBANG_

#include "bus.h"
#include "i2c.h"

#define    I2C_OK  0x0000
#define   I2C_ACK  0x0010
#define   I2C_NAK  0x0020

#define delay_s     { _NOP(); }
#define delay_c     { _NOP(); _NOP(); _NOP(); }

#ifdef __cplusplus
extern "C" {
#endif

/** initialize the i2c bitbanged bus by setting both scl and sda pins as inputs. external pullup resistors are required.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
*/
void sbb_i2cm_init(bus_desc_i2c_sw_master_t *i2c_bus_desc);

/** begin an i2c transaction defined in pkg. external pullup resistors are required. can block if slave holds CLK low or SDA high.
    @param i2c_bus_desc  pointer to a i2c bus descriptor defined by the bus_desc_i2c_sw_master_t struct
    @param pkg  pointer to a package struct that describes the transfer operation
    @return either I2C_ACK, I2C_NAK or a combination of I2C_MISSING_SDA_PULLUP, I2C_MISSING_SCL_PULLUP
*/
uint16_t sbb_i2cm_transfer(bus_desc_i2c_sw_master_t *i2c_bus_desc, const i2c_package_t *pkg);

#ifdef __cplusplus
}
#endif

#endif
