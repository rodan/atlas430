
//  library for ADS1110 - 16bit analog to digital converter
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/
//  license:         GNU GPLv3

#include "proj.h"
#include "ads1110.h"
#include "sys_messagebus.h"
#include "timer_a0.h"

#include "serial_bitbang.h"

#ifdef HARDWARE_I2C
    #include "drivers/i2c.h"
#endif

// doublecheck that CCR2 is not used by another driver
static void ads1110_state_machine(enum sys_message msg)
{
    switch (eadc.state) {
        case STATE_CONVERT:
            // conversion should be ready
            ads1110_read(ED0, &eadc);
            ads1110_convert(&eadc);
            eadc.state = STATE_STANDBY;
        break;
        default:
            // trigger new single conversion
            ads1110_config(ED0, BITS_16 | PGA_2 | SC | ST);
            eadc.state = STATE_CONVERT;
            timer_a0_delay_noblk_ccr2(_10ms * 7);
        break;
    }
}

uint8_t ads1110_init(const uint8_t slave_address, const uint8_t config_reg)
{
    sys_messagebus_register(&ads1110_state_machine, SYS_MSG_TIMER0_CCR2);
    ads1110_config(slave_address, config_reg);

    return EXIT_SUCCESS;
}

// returns  0 if all is fine
//          I2C err levels if sensor is not properly hooked up

uint8_t ads1110_read(const uint8_t slave_addr, struct ads1110 *adc)
{
    uint8_t val[3] = { 0, 0, 0 };
    //uint8_t rdy;

    i2c_package_t pkg;
    pkg.slave_addr = slave_addr;
    pkg.addr[0] = 0;
    pkg.addr_len = 0;
    pkg.data = val;
    pkg.data_len = 3;
    //pkg.read = 1;
    pkg.options = I2C_READ | I2C_LAST_NAK;

#ifdef HARDWARE_I2C
    i2c_transfer_start(&pkg, NULL);
#else
    uint8_t rv;
    rv = i2cm_transfer(&pkg);

    if (rv != I2C_ACK) {
        return rv;
    }
#endif
    adc->conv_raw = ( val[0] << 8 ) + val[1];
    adc->config = val[2];
    //rdy = (val[2] & 0x80) >> 7;
    return EXIT_SUCCESS;
}

void ads1110_convert(struct ads1110 *adc)
{
    uint8_t bits;
    uint8_t PGA; // gain setting (0-3)
    int32_t tmp;

    bits = (adc->config & 0xc) >> 2; // DR aka data rate
    if (bits == 0) {
        bits = 12;
    } else {
        bits += 13;
    }
    PGA = adc->config & 0x3;

    tmp = ((int32_t) adc->conv_raw * 20480) >> (bits - 1 + PGA);
    adc->conv = tmp;
}

// set the configuration register
uint8_t ads1110_config(const uint8_t slave_addr, const uint8_t val)
{

    uint8_t data = val;

    pkg.slave_addr = slave_addr;
    pkg.addr[0] = 0;
    pkg.addr_len = 0;
    pkg.data = &data;
    pkg.data_len = 1;
    //pkg.read = 0;
    pkg.options = I2C_WRITE;

#ifdef HARDWARE_I2C
    i2c_transfer_start(&pkg, NULL);
#else
    uint8_t rv;
    rv = i2cm_transfer(&pkg);
    return rv;
#endif

    return EXIT_SUCCESS;
}

