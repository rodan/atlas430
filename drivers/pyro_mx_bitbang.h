#ifndef __PYRO_MX_BITBANG_
#define __PYRO_MX_BITBANG_

#include "proj.h"

#define pyro_mx_act_high    PYRO_MX_DIR &= ~PYRO_MX_ACT
#define pyro_mx_act_low     PYRO_MX_DIR |= PYRO_MX_ACT

#define SSI_PYRO_OK                  0x0

volatile uint8_t pyro_mx_rx[5];
volatile uint16_t pyro_mx_p;

enum pyro_mx_tevent {
    PYRO_MX_RX = BIT0,
    PYRO_MX_RDY = BIT1
};

typedef struct {
    // remote temperature
    uint16_t rem_temp_raw;
    float rem_temp_float;
    // reference temperature
    uint16_t ref_temp_raw;
    float ref_temp_float;
} pyro_mx_t;

volatile enum pyro_mx_tevent pyro_mx_last_event;
pyro_mx_t pyro_mx;

// send a 'start' sequence
uint8_t i2c_pyro_mx_init(void);

#endif
