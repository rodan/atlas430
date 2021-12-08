
#include "i2c_ucb0_pin.h"
#include "i2c_ucb1_pin.h"

void i2c_pin_init(void)
{
#ifdef I2C_USES_UCB0
    i2c_ucb0_pin_init();
#endif

#ifdef I2C_USES_UCB1
    i2c_ucb1_pin_init();
#endif
}

