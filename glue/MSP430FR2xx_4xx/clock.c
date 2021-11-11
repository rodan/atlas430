
#include <msp430.h>
#include <inttypes.h>
#include "clock_selection.h"
#include "cs.h"
#include "framctl.h"
#include "clock.h"

// USE_XT1 has to be defined if a crystal is present

void clock_port_init(void)
{
#ifdef USE_XT1
    #ifdef __MSP430FR2355__
    P2SEL0 &= ~(BIT6 | BIT7);
    P2SEL1 |= BIT6 | BIT7;
    #else
    #error USE_XT1 was defined but pins not known in 'glue/MSP430FR2xx_4xx/clock.c'
    #endif
#endif
}

void clock_init(void)
{

    // set FRAM wait states before configuring MCLK
    FRAMCtl_configureWaitStateControl(CLK_FRAM_NWAITS);

#if 0
#ifdef USE_XT1
    CS_setExternalClockSource(ACLK_FREQ);
#endif

    // set up ACLK, SMCLK, MCLK

#ifdef USE_XT1
    // LFXTCLK is the source for ACLK
    CS_initClockSignal(CS_ACLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_1);
#endif

    // the DCO (internally controlled oscillator) is the source for MCLK and SMCLK
    CS_setDCOFreq(CLK_DCORSEL, CLK_DCOFSEL);
    CS_initClockSignal(CS_SMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // turn on 

#ifdef USE_XT1
    CS_turnOnXT1LF(CLK_LFXT_DRIVE);
#endif
#endif
}
