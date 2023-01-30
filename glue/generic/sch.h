#ifndef __SCH_H__
#define __SCH_H__

/**
* \addtogroup MAIN scheduler
* \brief Application-level simple scheduler
* \author Petre Rodan
* 
* \{
**/

/**
* \file
* \brief Include file for \ref SCHEDULER
* \author Petre Rodan
**/

//#include "proj.h"

// tick interval is about 10ms
// 327*(1/32768) = .009979248046875 s
#define            SCH_TICK  327

// 1ms in clock ticks
// 65535*0.001/(32/8000000*65535) = 0.001/(32/8000000) = 250

#define      SCH_EVENT_NONE  0x0
#define      SCH_EVENT_CCR1  0x1

#define    SCH_EVENT_ENABLE  0x1
#define   SCH_EVENT_DISABLE  0x0

#define     SCH_SLOTS_COUNT  16
#define          SCH_SLOT_0  0
#define          SCH_SLOT_1  1
#define          SCH_SLOT_2  2
#define          SCH_SLOT_3  3
#define          SCH_SLOT_4  4
#define          SCH_SLOT_5  5
#define          SCH_SLOT_6  6
#define          SCH_SLOT_7  7
#define          SCH_SLOT_8  8
#define          SCH_SLOT_9  9
#define         SCH_SLOT_10  10
#define         SCH_SLOT_11  11
#define         SCH_SLOT_12  12
#define         SCH_SLOT_13  13
#define         SCH_SLOT_14  14
#define         SCH_SLOT_15  15

#ifdef SCH_USES_T0A
    #if defined (TIMER0_A1_VECTOR)
        #define HAL_SCH_TIMER_VECTOR TIMER0_A1_VECTOR
    #else
        #error TIMER0_A1 interrupt vector name unknown
    #endif

    #if defined (TA0IV__TACCR1)
        #define HAL_SCH_TAxIV_TACCR1 TA0IV__TACCR1
    #elif defined (TA0IV_TACCR1)
        #define HAL_SCH_TAxIV_TACCR1 TA0IV_TACCR1
    #else
        #error TA0IV_TACCR1 interrupt vector register name unknown
    #endif

    #define HAL_SCH_TAxIV TA0IV
    #define HAL_SCH_TAxCCTL1 TA0CCTL1
    #define HAL_SCH_TAxCCR1 TA0CCR1
    #define HAL_SCH_TAxR TA0R
    #define HAL_SCH_TAxCTL TA0CTL
    #define HAL_SCH_TAxIV_TAxIFG TA0IV_TA0IFG
    #define HAL_SCH_TAxEX0 TA0EX0
#endif

#ifdef SCH_USES_T1A
    #if defined (TIMER1_A1_VECTOR)
        #define HAL_SCH_TIMER_VECTOR TIMER1_A1_VECTOR
    #else
        #error TIMER1_A1 interrupt vector name unknown
    #endif

    #if defined (TA1IV__TACCR1)
        #define HAL_SCH_TAxIV_TACCR1 TA1IV__TACCR1
    #elif defined (TA1IV_TACCR1)
        #define HAL_SCH_TAxIV_TACCR1 TA1IV_TACCR1
    #else
        #error TA1IV_TACCR1 interrupt vector register name unknown
    #endif

    #define HAL_SCH_TAxIV TA1IV
    #define HAL_SCH_TAxCCTL1 TA1CCTL1
    #define HAL_SCH_TAxCCR1 TA1CCR1
    #define HAL_SCH_TAxR TA1R
    #define HAL_SCH_TAxCTL TA1CTL
    #define HAL_SCH_TAxIV_TAxIFG TA1IV_TA1IFG
    #define HAL_SCH_TAxEX0 TA1EX0
#endif

#ifdef SCH_USES_T2A
    #if defined (TIMER2_A1_VECTOR)
        #define HAL_SCH_TIMER_VECTOR TIMER2_A1_VECTOR
    #else
        #error TIMER2_A1 interrupt vector name unknown
    #endif

    #if defined (TA2IV__TACCR1)
        #define HAL_SCH_TAxIV_TACCR1 TA2IV__TACCR1
    #elif defined (TA2IV_TACCR1)
        #define HAL_SCH_TAxIV_TACCR1 TA2IV_TACCR1
    #else
        #error TA2IV_TACCR1 interrupt vector register name unknown
    #endif

    #define HAL_SCH_TAxIV TA2IV
    #define HAL_SCH_TAxCCTL1 TA2CCTL1
    #define HAL_SCH_TAxCCR1 TA2CCR1
    #define HAL_SCH_TAxR TA2R
    #define HAL_SCH_TAxCTL TA2CTL
    #define HAL_SCH_TAxIV_TAxIFG TA2IV_TA2IFG
    #define HAL_SCH_TAxEX0 TA2EX0
#endif

typedef struct _sch_descriptor {
    uint16_t trigger_schedule;  ///< bitwise flag of which slots are currently active
    uint32_t trigger_next;      ///< tick when this scheduler is supposed to trigger next
    uint32_t trigger_slot[SCH_SLOTS_COUNT]; ///< systimes at which slots are set to trigger
    uint16_t last_event;        ///< bitwise flag of current timer_a2 interrupt events
    uint16_t last_event_schedule;        ///< bitwise flag of which slots have currently triggered
    uint32_t systime;           ///< time in ticks (10ms increments) since scheduler started
} sch_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t sch_init(void);

uint16_t sch_get_event(void);
void sch_rst_event(void);
uint16_t sch_get_event_schedule(void);
void sch_rst_event_schedule(void);
uint32_t sch_get_trigger_next(void);

uint32_t systime(void);

uint8_t sch_set_trigger_slot(const uint16_t slot, const uint32_t trigger, const uint8_t flag);
uint8_t sch_get_trigger_slot(const uint16_t slot, uint32_t *trigger, uint8_t *flag);
void sch_handler(void);

#ifdef __cplusplus
}
#endif

#endif
///\}
