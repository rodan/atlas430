
// glue for Olimex MOD-rs485-ISO board


#include "serial_bitbang.h"
#include "mod_rs485_iso.h"


uint8_t mod_rs485_iso_tx_cmd(const uint8_t addr, const uint8_t command, const uint8_t arg)
{
    uint8_t rv;

    rv = i2cm_start();
    if (rv == I2C_OK) {
        i2cm_tx(0x48, I2C_WRITE); // company
        i2cm_tx(0x04, I2C_NO_ADDR_SHIFT); // select rs485-iso device
        i2cm_tx(addr, I2C_NO_ADDR_SHIFT); // device id
        i2cm_tx(command, I2C_NO_ADDR_SHIFT);
        rv += i2cm_tx(arg, I2C_NO_ADDR_SHIFT);
    }

    i2cm_stop();
    return rv;
}


