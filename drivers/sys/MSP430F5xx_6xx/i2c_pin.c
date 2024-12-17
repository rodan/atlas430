
#include "i2c_ucb0_pin.h"
#include "i2c_ucb1_pin.h"
#include "i2c_ucb2_pin.h"
#include "i2c_ucb3_pin.h"

void i2c_pin_init(void)
{
#ifdef I2C_USES_UCB0
    i2c_ucb0_pin_init();
#endif

#ifdef I2C_USES_UCB1
    i2c_ucb1_pin_init();
#endif

#ifdef I2C_USES_UCB2
    i2c_ucb2_pin_init();
#endif

#ifdef I2C_USES_UCB3
    i2c_ucb3_pin_init();
#endif

//#if ! defined I2C_USES_UCB0 && ! defined I2C_USES_UCB1 && ! defined I2C_USES_UCB2 && ! defined I2C_USES_UCB3
//    #error I2C_USES_UCBx not defined
//#endif
}

