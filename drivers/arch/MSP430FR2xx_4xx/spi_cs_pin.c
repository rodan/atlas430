
#include "spi_cs_uca0_pin.h"
#include "spi_cs_uca1_pin.h"
#include "spi_cs_uca2_pin.h"
#include "spi_cs_uca3_pin.h"
#include "spi_cs_ucb0_pin.h"
#include "spi_cs_ucb1_pin.h"
#include "spi_cs_ucb2_pin.h"
#include "spi_cs_ucb3_pin.h"

void spi_cs_pin_init(void)
{
#ifdef SPI_USES_UCA0
    spi_cs_uca0_pin_init();
#endif

#ifdef SPI_USES_UCA1
    spi_cs_uca1_pin_init();
#endif

#ifdef SPI_USES_UCA2
    spi_cs_uca2_pin_init();
#endif

#ifdef SPI_USES_UCA3
    spi_cs_uca3_pin_init();
#endif

#ifdef SPI_USES_UCB0
    spi_cs_ucb0_pin_init();
#endif

#ifdef SPI_USES_UCB1
    spi_cs_ucb1_pin_init();
#endif

#ifdef SPI_USES_UCB2
    spi_cs_ucb2_pin_init();
#endif

#ifdef SPI_USES_UCB3
    spi_cs_ucb3_pin_init();
#endif

}

