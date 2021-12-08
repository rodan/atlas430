/*
  pin setup for the i2c UCB1 subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on Wed Dec  8 10:10:18 UTC 2021
*/

#include <msp430.h>

void i2c_ucb1_pin_init(void)
{

#ifdef I2C_USES_UCB1

#if defined (__MSP430FR2153__) || defined (__MSP430FR2155__) \
 || defined (__MSP430FR2353__) || defined (__MSP430FR2355__)

    P4SEL0 |= BIT6 | BIT7;
    P4SEL1 &= ~(BIT6 | BIT7);

#elif defined (__MSP430FR2475__) || defined (__MSP430FR2476__) \
 || defined (__MSP430FR2672__) || defined (__MSP430FR2673__) \
 || defined (__MSP430FR2675__) || defined (__MSP430FR2676__)

    P3SEL0 |= BIT2 | BIT6;
    P3SEL1 &= ~(BIT2 | BIT6);
    P4SEL0 |= BIT3 | BIT4;
    P4SEL1 &= ~(BIT3 | BIT4);
#warning multiple pins found for the UCB1SCL function, you must initialize them manually
#warning multiple pins found for the UCB1SDA function, you must initialize them manually

#elif defined (__MSP430FR2000__) || defined (__MSP430FR2032__) \
 || defined (__MSP430FR2033__) || defined (__MSP430FR2100__) \
 || defined (__MSP430FR2110__) || defined (__MSP430FR2111__) \
 || defined (__MSP430FR2310__) || defined (__MSP430FR2311__) \
 || defined (__MSP430FR2422__) || defined (__MSP430FR2433__) \
 || defined (__MSP430FR2512__) || defined (__MSP430FR2522__) \
 || defined (__MSP430FR2532__) || defined (__MSP430FR2533__) \
 || defined (__MSP430FR2632__) || defined (__MSP430FR2633__) \
 || defined (__MSP430FR4131__) || defined (__MSP430FR4132__) \
 || defined (__MSP430FR4133__)

#warning function UCB1SCL not found for this uC
#warning function UCB1SDA not found for this uC

#else
    #warning "I2C_USES_UCB1 was defined but pins not known in 'glue/MSP430FR2xx_4xx/i2c_ucb1_pin.c'"
#endif

#endif
}
