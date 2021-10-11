
#include <msp430.h>
#include <inttypes.h>
#include "pmm.h"
#include "clock_selection.h"
#include "ucs.h"
#include "clock.h"

// USE_XT1 has to be defined if the LFXTCLK (32768 Hz) crystal is present
// USE_XT2 has to be defined if the HFXTCLK crystal is present

void clock_port_init(void)
{
#ifdef USE_XT1
    #if defined (__MSP430F5510__) || defined (__MSP430F5529__)
        P5SEL |= BIT4 | BIT5;
    #elif defined (__MSP430F5438__)
        P7SEL |= BIT0 | BIT1;
    #else
    #error unknown pinout. modify clock.c
    #endif
#endif

#ifdef USE_XT2
    #if defined (__MSP430F5510__) || defined (__MSP430F5529__) || defined (__MSP430F5438__)
        P5SEL |= BIT2 | BIT3;
    #else
    #error unknown pinout. modify clock.c
    #endif
#endif
}

void clock_init(void)
{

#ifdef USE_XT1
    #ifdef USE_XT2
        UCS_setExternalClockSource(ACLK_FREQ, SMCLK_FREQ);
    #else
        UCS_setExternalClockSource(ACLK_FREQ, 0);
    #endif
#else
    #ifdef USE_XT2
        UCS_setExternalClockSource(0, SMCLK_FREQ);
    #endif
#endif

    // set up ACLK, SMCLK, MCLK

#ifdef USE_XT1
    // LFXTCLK is the source for ACLK, FLL
    UCS_initClockSignal(UCS_ACLK, UCS_XT1CLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initClockSignal(UCS_FLLREF, UCS_XT1CLK_SELECT, UCS_CLOCK_DIVIDER_1);
#else
    // REFOCLK is the source for ACLK, FLL
    UCS_initClockSignal(UCS_ACLK, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initClockSignal(UCS_FLLREF, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
#endif

    PMM_setVCore(PMM_CORE_LVL);

#ifdef USE_XT2
    // HFXTCLK is the source for MCLK and SMCLK
    UCS_initClockSignal(UCS_SMCLK, UCS_XT2CLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initClockSignal(UCS_MCLK, UCS_XT2CLK_SELECT, UCS_CLOCK_DIVIDER_1);
#else
    // the DCO (internally controlled oscillator) is the source for MCLK and MCLK
    //UCS_initClockSignal(UCS_SMCLK, UCS_DCOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
    //UCS_initClockSignal(UCS_MCLK, UCS_DCOCLK_SELECT, UCS_CLOCK_DIVIDER_1);
    UCS_initFLLSettle(SMCLK_FREQ / 1000, SMCLK_FREQ / ACLK_FREQ);
#endif

    // turn on 
#ifdef USE_XT1
    UCS_turnOnLFXT1(CLK_LFXT_DRIVE, UCS_XCAP_0);
#endif

#ifdef USE_XT2
    UCS_turnOnXT2(CLK_HFXT_DRIVE);
#endif

}
