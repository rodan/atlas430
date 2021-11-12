#ifndef __CLOCK_SELECTION_H__
#define __CLOCK_SELECTION_H__

#include "pmm.h"

#ifdef __cplusplus
extern "C" {
#endif

// make sure to define one of SMCLK_FREQ_xM in config.h

#if defined(SMCLK_FREQ_1M)
    #define      SMCLK_FREQ  1000000
    #define  CLK_HFXT_DRIVE  UCS_XT2_DRIVE_4MHZ_8MHZ
    #define    PMM_CORE_LVL  PMM_CORE_LEVEL_0
#elif defined(SMCLK_FREQ_4M)
    #define      SMCLK_FREQ  4000000
    #define  CLK_HFXT_DRIVE  UCS_XT2_DRIVE_4MHZ_8MHZ
    #define    PMM_CORE_LVL  PMM_CORE_LEVEL_0
#elif defined(SMCLK_FREQ_8M)
    #define      SMCLK_FREQ  8000000
    #define  CLK_HFXT_DRIVE  UCS_XT2_DRIVE_8MHZ_16MHZ
    #define    PMM_CORE_LVL  PMM_CORE_LEVEL_1
#elif defined(SMCLK_FREQ_16M)
    #define      SMCLK_FREQ  16000000
    #define  CLK_HFXT_DRIVE  UCS_XT2_DRIVE_16MHZ_24MHZ
    #define    PMM_CORE_LVL  PMM_CORE_LEVEL_2
#elif defined(SMCLK_FREQ_24M)
    #define      SMCLK_FREQ  24000000
    #define  CLK_HFXT_DRIVE  UCS_XT2_DRIVE_24MHZ_32MHZ
    #define    PMM_CORE_LVL  PMM_CORE_LEVEL_3
#elif defined(SMCLK_FREQ_32M)
    #define      SMCLK_FREQ  32000000
    #define  CLK_HFXT_DRIVE  UCS_XT2_DRIVE_24MHZ_32MHZ
    #define    PMM_CORE_LVL  PMM_CORE_LEVEL_3
#else
    #error SMCLK_FREQ_xM not defined in config.h
#endif

#if SMCLK_FREQ > 20000000
    #if defined (__CC430F5137__)
    #error "unsupported clock frequency for this IC"
    #endif
#endif

#if SMCLK_FREQ > 25000000
    #if defined (__MSP430F5529__)
    #error "unsupported clock frequency for this IC"
    #endif
#endif

#ifdef __cplusplus
}
#endif

#endif
