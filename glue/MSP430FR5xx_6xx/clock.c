
#include <msp430.h>
#include <inttypes.h>
#include "clock_selection.h"
#include "cs.h"
#include "framctl.h"
#include "clock.h"

// USE_XT1 has to be defined if the LFXTCLK (32768 Hz) crystal is present
// USE_XT2 has to be defined if the HFXTCLK crystal is present

void clock_port_init(void)
{
#ifdef USE_XT1
    PJSEL0 |= BIT4 | BIT5;
#endif

#ifdef USE_XT2
    PJSEL0 |= BIT6 | BIT7;
#endif
}

void clock_init(void)
{

    // set FRAM wait states before configuring MCLK
    FRAMCtl_configureWaitStateControl(CLK_FRAM_NWAITS);

#ifdef USE_XT1
    #ifdef USE_XT2
        CS_setExternalClockSource(ACLK_FREQ, SMCLK_FREQ);
    #else
        CS_setExternalClockSource(ACLK_FREQ, 0);
    #endif
#else
    #ifdef USE_XT2
        CS_setExternalClockSource(0, SMCLK_FREQ);
    #endif
#endif

    // set up ACLK, SMCLK, MCLK

#ifdef USE_XT1
    // LFXTCLK is the source for ACLK
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
#endif

#ifdef USE_XT2
    // HFXTCLK is the source for MCLK and SMCLK
    CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
#else
    // the DCO (internally controlled oscillator) is the source for MCLK and SMCLK
    CS_setDCOFreq(CLK_DCORSEL, CLK_DCOFSEL);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
#endif

    // turn on 

#ifdef USE_XT1
    CS_turnOnLFXT(CLK_LFXT_DRIVE);
#endif

#ifdef USE_XT2
    CS_turnOnHFXT(CLK_HFXT_DRIVE);
#endif

}
