/*
  pin setup for the spi UCA3 subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on Tue Jan  4 14:24:27 UTC 2022
*/

#include <msp430.h>

void spi_uca3_pin_init(void)
{

#ifdef SPI_USES_UCA3

#if defined (__MSP430FR5962__) || defined (__MSP430FR5964__) \
 || defined (__MSP430FR5992__) || defined (__MSP430FR59941__) \
 || defined (__MSP430FR5994__)

    P6SEL0 |= BIT0 | BIT1 | BIT2;
    P6SEL1 &= ~(BIT0 | BIT1 | BIT2);

#elif defined (__MSP430FR5041__) || defined (__MSP430FR50431__) \
 || defined (__MSP430FR5043__) || defined (__MSP430FR6041__) \
 || defined (__MSP430FR60431__) || defined (__MSP430FR6043__)

    P1SEL0 |= BIT7;
    P1SEL1 &= ~BIT7;
    P2SEL0 &= ~(BIT0 | BIT1);
    P2SEL1 |= BIT0 | BIT1;
    P4SEL0 |= BIT1 | BIT2;
    P4SEL1 |= BIT1 | BIT2;
#warning multiple pins found for the UCA3SIMO function, you must initialize them manually
#warning multiple pins found for the UCA3SOMI function, you must initialize them manually

#elif defined (__MSP430FR58471__) || defined (__MSP430FR5847__) \
 || defined (__MSP430FR5848__) || defined (__MSP430FR5849__) \
 || defined (__MSP430FR5857__) || defined (__MSP430FR5858__) \
 || defined (__MSP430FR5859__) || defined (__MSP430FR58671__) \
 || defined (__MSP430FR5867__) || defined (__MSP430FR5868__) \
 || defined (__MSP430FR5869__) || defined (__MSP430FR5870__) \
 || defined (__MSP430FR58721__) || defined (__MSP430FR5872__) \
 || defined (__MSP430FR5887__) || defined (__MSP430FR5888__) \
 || defined (__MSP430FR58891__) || defined (__MSP430FR5889__) \
 || defined (__MSP430FR59221__) || defined (__MSP430FR5922__) \
 || defined (__MSP430FR59471__) || defined (__MSP430FR5947__) \
 || defined (__MSP430FR5948__) || defined (__MSP430FR5949__) \
 || defined (__MSP430FR5957__) || defined (__MSP430FR5958__) \
 || defined (__MSP430FR5959__) || defined (__MSP430FR5967__) \
 || defined (__MSP430FR5968__) || defined (__MSP430FR59691__) \
 || defined (__MSP430FR5969__) || defined (__MSP430FR5970__) \
 || defined (__MSP430FR59721__) || defined (__MSP430FR5972__) \
 || defined (__MSP430FR5986__) || defined (__MSP430FR5987__) \
 || defined (__MSP430FR5988__) || defined (__MSP430FR59891__) \
 || defined (__MSP430FR5989__) || defined (__MSP430FR6820__) \
 || defined (__MSP430FR68221__) || defined (__MSP430FR6822__) \
 || defined (__MSP430FR6870__) || defined (__MSP430FR68721__) \
 || defined (__MSP430FR6872__) || defined (__MSP430FR6877__) \
 || defined (__MSP430FR68791__) || defined (__MSP430FR6879__) \
 || defined (__MSP430FR6887__) || defined (__MSP430FR6888__) \
 || defined (__MSP430FR68891__) || defined (__MSP430FR6889__) \
 || defined (__MSP430FR6920__) || defined (__MSP430FR69221__) \
 || defined (__MSP430FR6922__) || defined (__MSP430FR69271__) \
 || defined (__MSP430FR6927__) || defined (__MSP430FR6928__) \
 || defined (__MSP430FR6970__) || defined (__MSP430FR69721__) \
 || defined (__MSP430FR6972__) || defined (__MSP430FR6977__) \
 || defined (__MSP430FR69791__) || defined (__MSP430FR6979__) \
 || defined (__MSP430FR6987__) || defined (__MSP430FR6988__) \
 || defined (__MSP430FR69891__) || defined (__MSP430FR6989__)

#warning function UCA3CLK not found for this uC
#warning function UCA3SIMO not found for this uC
#warning function UCA3SOMI not found for this uC

#elif defined (__MSP430FR6005__) || defined (__MSP430FR6007__) \
 || defined (__MSP430FR6035__) || defined (__MSP430FR60371__) \
 || defined (__MSP430FR6037__) || defined (__MSP430FR6045__) \
 || defined (__MSP430FR60471__) || defined (__MSP430FR6047__)

    P8SEL0 |= BIT1 | BIT2 | BIT3;
    P8SEL1 &= ~(BIT1 | BIT2 | BIT3);

#else
    #warning "SPI_USES_UCA3 was defined but pins not known in 'glue/MSP430FR5xx_6xx/spi_uca3_pin.c'"
#endif

#endif
}
