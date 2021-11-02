#ifndef __SENSIRION_H__
#define __SENSIRION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// coefficients for 12bit rh, 14bit temperature, 3.3V Vdd
#define C1      -2.0468
#define C2       0.0367
#define C3      -1.5955E-6
#define T1       0.01
#define T2       0.00008
#define D1     -39.7
#define D2       0.01

uint8_t SHT1X_get_status(uint8_t * data);
uint8_t SHT1X_get_meas(int16_t * temp, uint16_t * rh);

// reset connection on the pseudo i2c protocol
void SHT1X_i2csens_reset(void);

#ifdef __cplusplus
}
#endif

#endif
