
#include <msp430.h>
#include <inttypes.h>
#include "clock_selection.h"
#include "cs.h"
#include "framctl.h"
#include "clock_pin.h"
#include "clock.h"

// USE_XT1 has to be defined if a crystal is present

void clock_init(void)
{
    clock_pin_init();

    // set FRAM wait states before configuring MCLK
    FRAMCtl_configureWaitStateControl(CLK_FRAM_NWAITS);

    // if XT1 is used, ONLY a 32478 Hz crystal is expected in it
#ifdef USE_XT1
    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);           // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);                   // Test oscillator fault flag

    CS_setExternalClockSource(ACLK_FREQ);
    CS_initClockSignal(CS_FLLREF, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initFLL(SMCLK_FREQ / 1000, SMCLK_FREQ / ACLK_FREQ);
    CS_initClockSignal(CS_ACLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_1);
#else
    CS_initClockSignal(CS_FLLREF, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initFLL(SMCLK_FREQ / 1000, SMCLK_FREQ / ACLK_FREQ);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
#endif

    CS_initClockSignal(CS_SMCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);

#ifdef USE_XT1
    CS_turnOnXT1LF(CLK_LFXT_DRIVE);
#endif
}
