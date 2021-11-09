#ifndef __CLOCK_SELECTION_H__
#define __CLOCK_SELECTION_H__

#ifdef __cplusplus
extern "C" {
#endif

// make sure to define one of SMCLK_FREQ_xM in config.h

#if defined(SMCLK_FREQ_1M)
    #define      SMCLK_FREQ  1000000
    #define     CLK_DCORSEL  CS_DCORSEL_0
    #define     CLK_DCOFSEL  CS_DCOFSEL_0
    #define  CLK_HFXT_DRIVE  CS_HFXT_DRIVE_4MHZ_8MHZ
#elif defined(SMCLK_FREQ_4M)
    #define      SMCLK_FREQ  4000000
    #define     CLK_DCORSEL  CS_DCORSEL_0
    #define     CLK_DCOFSEL  CS_DCOFSEL_3
    #define  CLK_HFXT_DRIVE  CS_HFXT_DRIVE_4MHZ_8MHZ
#elif defined(SMCLK_FREQ_8M)
    #define      SMCLK_FREQ  8000000
    #define     CLK_DCORSEL  CS_DCORSEL_0
    #define     CLK_DCOFSEL  CS_DCOFSEL_6
    #define  CLK_HFXT_DRIVE  CS_HFXT_DRIVE_8MHZ_16MHZ
#elif defined(SMCLK_FREQ_16M)
    #define      SMCLK_FREQ  16000000
    #define     CLK_DCORSEL  CS_DCORSEL_1
    #define     CLK_DCOFSEL  CS_DCOFSEL_4
    #define  CLK_HFXT_DRIVE  CS_HFXT_DRIVE_16MHZ_24MHZ
#else
    #error SMCLK_FREQ_xM not defined in config.h
#endif

#ifdef __cplusplus
}
#endif

#endif
