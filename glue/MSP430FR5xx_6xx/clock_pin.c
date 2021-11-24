/*
  pin setup for crystal-based clocks. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430

  generated on Wed Nov 24 09:37:00 UTC 2021
*/

#include <msp430.h>

void clock_pin_init(void)
{
#ifdef USE_XT1

#if defined (__MSP430FR2433__) || defined (__MSP430FR2475__) \
 || defined (__MSP430FR2476__) || defined (__MSP430FR2532__) \
 || defined (__MSP430FR2533__) || defined (__MSP430FR2632__) \
 || defined (__MSP430FR2633__) || defined (__MSP430FR2672__) \
 || defined (__MSP430FR2673__) || defined (__MSP430FR2675__) \
 || defined (__MSP430FR2676__)

    P2SEL0 |= BIT0 | BIT1;
    P2SEL1 &= ~(BIT0 | BIT1);

#elif defined (__MSP430FR5041__) || defined (__MSP430FR50431__) \
 || defined (__MSP430FR5043__) || defined (__MSP430FR58471__) \
 || defined (__MSP430FR5847__) || defined (__MSP430FR5848__) \
 || defined (__MSP430FR5849__) || defined (__MSP430FR5857__) \
 || defined (__MSP430FR5858__) || defined (__MSP430FR5859__) \
 || defined (__MSP430FR58671__) || defined (__MSP430FR5867__) \
 || defined (__MSP430FR5868__) || defined (__MSP430FR5869__) \
 || defined (__MSP430FR5870__) || defined (__MSP430FR58721__) \
 || defined (__MSP430FR5872__) || defined (__MSP430FR5887__) \
 || defined (__MSP430FR5888__) || defined (__MSP430FR58891__) \
 || defined (__MSP430FR5889__) || defined (__MSP430FR59221__) \
 || defined (__MSP430FR5922__) || defined (__MSP430FR59471__) \
 || defined (__MSP430FR5947__) || defined (__MSP430FR5948__) \
 || defined (__MSP430FR5949__) || defined (__MSP430FR5957__) \
 || defined (__MSP430FR5958__) || defined (__MSP430FR5959__) \
 || defined (__MSP430FR5962__) || defined (__MSP430FR5964__) \
 || defined (__MSP430FR5967__) || defined (__MSP430FR5968__) \
 || defined (__MSP430FR59691__) || defined (__MSP430FR5969__) \
 || defined (__MSP430FR5970__) || defined (__MSP430FR59721__) \
 || defined (__MSP430FR5972__) || defined (__MSP430FR5986__) \
 || defined (__MSP430FR5987__) || defined (__MSP430FR5988__) \
 || defined (__MSP430FR59891__) || defined (__MSP430FR5989__) \
 || defined (__MSP430FR5992__) || defined (__MSP430FR59941__) \
 || defined (__MSP430FR5994__) || defined (__MSP430FR6005__) \
 || defined (__MSP430FR6007__) || defined (__MSP430FR6035__) \
 || defined (__MSP430FR60371__) || defined (__MSP430FR6037__) \
 || defined (__MSP430FR6041__) || defined (__MSP430FR60431__) \
 || defined (__MSP430FR6043__) || defined (__MSP430FR6045__) \
 || defined (__MSP430FR60471__) || defined (__MSP430FR6047__) \
 || defined (__MSP430FR6820__) || defined (__MSP430FR68221__) \
 || defined (__MSP430FR6822__) || defined (__MSP430FR6870__) \
 || defined (__MSP430FR68721__) || defined (__MSP430FR6872__) \
 || defined (__MSP430FR6877__) || defined (__MSP430FR68791__) \
 || defined (__MSP430FR6879__) || defined (__MSP430FR6887__) \
 || defined (__MSP430FR6888__) || defined (__MSP430FR68891__) \
 || defined (__MSP430FR6889__) || defined (__MSP430FR6920__) \
 || defined (__MSP430FR69221__) || defined (__MSP430FR6922__) \
 || defined (__MSP430FR69271__) || defined (__MSP430FR6927__) \
 || defined (__MSP430FR6928__) || defined (__MSP430FR6970__) \
 || defined (__MSP430FR69721__) || defined (__MSP430FR6972__) \
 || defined (__MSP430FR6977__) || defined (__MSP430FR69791__) \
 || defined (__MSP430FR6979__) || defined (__MSP430FR6987__) \
 || defined (__MSP430FR6988__) || defined (__MSP430FR69891__) \
 || defined (__MSP430FR6989__)

    PJSEL0 |= BIT4;
    PJSEL1 &= ~BIT4;

#elif defined (__MSP430FR2000__) || defined (__MSP430FR2100__) \
 || defined (__MSP430FR2110__) || defined (__MSP430FR2111__) \
 || defined (__MSP430FR2153__) || defined (__MSP430FR2155__) \
 || defined (__MSP430FR2310__) || defined (__MSP430FR2311__) \
 || defined (__MSP430FR2353__) || defined (__MSP430FR2355__)

    P2SEL0 &= ~(BIT6 | BIT7);
    P2SEL1 |= BIT6 | BIT7;

#elif defined (__MSP430FR2032__) || defined (__MSP430FR2033__) \
 || defined (__MSP430FR4131__) || defined (__MSP430FR4132__) \
 || defined (__MSP430FR4133__)

    P4SEL0 |= BIT1 | BIT2;

#elif defined (__MSP430FR2422__) || defined (__MSP430FR2512__) \
 || defined (__MSP430FR2522__)

    P2SEL0 &= ~(BIT0 | BIT1);
    P2SEL1 |= BIT0 | BIT1;

#else
    #error "USE_XT1 was defined but pins not known in 'glue/MSP430FR5xx_6xx/clock_pin.c'"
#endif
#endif

#ifdef USE_XT2
#if defined (__MSP430FR5041__) || defined (__MSP430FR50431__) \
 || defined (__MSP430FR5043__) || defined (__MSP430FR58471__) \
 || defined (__MSP430FR5847__) || defined (__MSP430FR5848__) \
 || defined (__MSP430FR5849__) || defined (__MSP430FR5857__) \
 || defined (__MSP430FR5858__) || defined (__MSP430FR5859__) \
 || defined (__MSP430FR58671__) || defined (__MSP430FR5867__) \
 || defined (__MSP430FR5868__) || defined (__MSP430FR5869__) \
 || defined (__MSP430FR5870__) || defined (__MSP430FR58721__) \
 || defined (__MSP430FR5872__) || defined (__MSP430FR5887__) \
 || defined (__MSP430FR5888__) || defined (__MSP430FR58891__) \
 || defined (__MSP430FR5889__) || defined (__MSP430FR59221__) \
 || defined (__MSP430FR5922__) || defined (__MSP430FR59471__) \
 || defined (__MSP430FR5947__) || defined (__MSP430FR5948__) \
 || defined (__MSP430FR5949__) || defined (__MSP430FR5957__) \
 || defined (__MSP430FR5958__) || defined (__MSP430FR5959__) \
 || defined (__MSP430FR5962__) || defined (__MSP430FR5964__) \
 || defined (__MSP430FR5967__) || defined (__MSP430FR5968__) \
 || defined (__MSP430FR59691__) || defined (__MSP430FR5969__) \
 || defined (__MSP430FR5970__) || defined (__MSP430FR59721__) \
 || defined (__MSP430FR5972__) || defined (__MSP430FR5986__) \
 || defined (__MSP430FR5987__) || defined (__MSP430FR5988__) \
 || defined (__MSP430FR59891__) || defined (__MSP430FR5989__) \
 || defined (__MSP430FR5992__) || defined (__MSP430FR59941__) \
 || defined (__MSP430FR5994__) || defined (__MSP430FR6005__) \
 || defined (__MSP430FR6007__) || defined (__MSP430FR6035__) \
 || defined (__MSP430FR60371__) || defined (__MSP430FR6037__) \
 || defined (__MSP430FR6041__) || defined (__MSP430FR60431__) \
 || defined (__MSP430FR6043__) || defined (__MSP430FR6045__) \
 || defined (__MSP430FR60471__) || defined (__MSP430FR6047__) \
 || defined (__MSP430FR6820__) || defined (__MSP430FR68221__) \
 || defined (__MSP430FR6822__) || defined (__MSP430FR6870__) \
 || defined (__MSP430FR68721__) || defined (__MSP430FR6872__) \
 || defined (__MSP430FR6877__) || defined (__MSP430FR68791__) \
 || defined (__MSP430FR6879__) || defined (__MSP430FR6887__) \
 || defined (__MSP430FR6888__) || defined (__MSP430FR68891__) \
 || defined (__MSP430FR6889__) || defined (__MSP430FR6920__) \
 || defined (__MSP430FR69221__) || defined (__MSP430FR6922__) \
 || defined (__MSP430FR69271__) || defined (__MSP430FR6927__) \
 || defined (__MSP430FR6928__) || defined (__MSP430FR6970__) \
 || defined (__MSP430FR69721__) || defined (__MSP430FR6972__) \
 || defined (__MSP430FR6977__) || defined (__MSP430FR69791__) \
 || defined (__MSP430FR6979__) || defined (__MSP430FR6987__) \
 || defined (__MSP430FR6988__) || defined (__MSP430FR69891__) \
 || defined (__MSP430FR6989__)

    PJSEL0 |= BIT6;
    PJSEL1 &= ~BIT6;

#else
    #error "USE_XT2 was defined but pins not known in 'glue/MSP430FR5xx_6xx/clock_pin.c'"
#endif
#endif
}
