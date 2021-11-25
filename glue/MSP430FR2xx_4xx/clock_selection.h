#ifndef __CLOCK_SELECTION_H__
#define __CLOCK_SELECTION_H__

#ifdef __cplusplus
extern "C" {
#endif

// make sure to define one of SMCLK_FREQ_xM in config.h

#if defined(SMCLK_FREQ_1M)
    #define      SMCLK_FREQ  1000000
    #define CLK_FRAM_NWAITS  NWAITS_0
#elif defined(SMCLK_FREQ_2M)
    #define      SMCLK_FREQ  2000000
    #define CLK_FRAM_NWAITS  NWAITS_0
#elif defined(SMCLK_FREQ_4M)
    #define      SMCLK_FREQ  4000000
    #define CLK_FRAM_NWAITS  NWAITS_0
#elif defined(SMCLK_FREQ_8M)
    #define      SMCLK_FREQ  8000000
    #define CLK_FRAM_NWAITS  NWAITS_0
#elif defined(SMCLK_FREQ_12M)
    #define      SMCLK_FREQ  12000000
    #define CLK_FRAM_NWAITS  NWAITS_1
#elif defined(SMCLK_FREQ_16M)
    #define      SMCLK_FREQ  16000000
    #define CLK_FRAM_NWAITS  NWAITS_1
#elif defined(SMCLK_FREQ_20M)
    #define      SMCLK_FREQ  20000000
    #define CLK_FRAM_NWAITS  NWAITS_2
#elif defined(SMCLK_FREQ_24M)
    #define      SMCLK_FREQ  24000000
    #define CLK_FRAM_NWAITS  NWAITS_2
#else
    #error SMCLK_FREQ_xM not defined in config.h
#endif

#if SMCLK_FREQ > 16000000
    #if defined (__MSP430FR2433__) || (__MSP430FR2476__) || (__MSP430FR4133__)
    #error "unsupported clock frequency for this IC"
    #endif
#endif

#ifdef __cplusplus
}
#endif

#endif
