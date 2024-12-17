
//   timer a2 handling
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/atlas430
//   license:         BSD

#if defined(SCH_USES_T0A) || defined(SCH_USES_T1A) || defined(SCH_USES_T2A) || defined(SCH_USES_T3A) || defined(SCH_USES_T0B) || defined(SCH_USES_T1B) || defined(SCH_USES_T2B) || defined(SCH_USES_T3B)

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "sch.h"

static sch_descriptor schd;

void sch_init(void)
{
    __disable_interrupt();
    HAL_SCH_CTL = HAL_SCH_SSEL__ACLK + MC__CONTINOUS + HAL_SCH_CLR;
    HAL_SCH_EX0 = 0;
    __enable_interrupt();

    // ccr1 will count up to ~10ms
    HAL_SCH_CCTL1 = 0;
    HAL_SCH_CCR1 = HAL_SCH_R + SCH_TICK;
    HAL_SCH_CCTL1 = CCIE;

    schd.systime = 0;
    schd.trigger_en = 0;
    schd.trigger_next = 0;
    schd.last_event = SCH_EVENT_NONE;
    schd.systime = 0;
}

uint8_t sch_set_trigger_slot(const uint16_t slot, const uint32_t ticks, const uint8_t flag)
{
    if (slot > SCH_SLOT_CNT - 1) {
        return EXIT_FAILURE;
    }

    schd.trigger_slot[slot] = ticks;
    if (flag == SCH_EVENT_ENABLE) {
        schd.trigger_en |= (1UL << slot);

        if (schd.trigger_next > ticks) {
            schd.trigger_next = ticks;
        }
    } else {
        schd.trigger_en &= ~(1UL << slot);
    }

    return EXIT_SUCCESS;
}

uint8_t sch_get_trigger_slot(const uint16_t slot, uint32_t * ticks, uint8_t * flag)
{
    if (slot > SCH_SLOT_CNT - 1) {
        return EXIT_FAILURE;
    }

    *ticks = schd.trigger_slot[slot];
    if (schd.trigger_en & (1UL << slot)) {
        *flag = 1;
    } else {
        *flag = 0;
    }

    return EXIT_SUCCESS;
}

uint16_t sch_get_event(void)
{
    return schd.last_event;
}

void sch_rst_event(void)
{
    schd.last_event = SCH_EVENT_NONE;
}

uint16_t sch_get_event_schedule(void)
{
    return schd.last_event_schedule;
}

void sch_rst_event_schedule(void)
{
    schd.last_event_schedule = SCH_EVENT_NONE;
}

uint32_t sch_get_trigger_next(void)
{
    return schd.trigger_next;
}

uint32_t systime(void)
{
    return schd.systime;
}

void sch_handler(void)
{
    uint16_t c;
    uint16_t shift;
    uint32_t schedule_trigger_next = 0;

    if (schd.trigger_en) {
        for (c = 0; c < SCH_SLOT_CNT; c++) {
            shift = 1UL << c;
            if (schd.trigger_en & shift) {
                // signal if event time was reached
                if (schd.trigger_slot[c] <= schd.systime) {
                    schd.trigger_en &= ~shift;
                    schd.last_event_schedule |= shift;
                } else if (schd.trigger_slot[c] < schedule_trigger_next) {
                    // prepare the next trigger
                    schedule_trigger_next = schd.trigger_slot[c];
                }
            }
        }
        schd.trigger_next = schedule_trigger_next;
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=HAL_SCH_TIMER_VECTOR
__interrupt void SCH_TIMER_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(HAL_SCH_TIMER_VECTOR))) SCH_TIMER_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = HAL_SCH_IV;

    if (iv == HAL_SCH_IV_CCR1) {
        HAL_SCH_CCTL1 &= ~CCIE;
        HAL_SCH_CCTL1 = 0;
        HAL_SCH_CCR1 = HAL_SCH_R + SCH_TICK;
        HAL_SCH_CCTL1 = CCIE;
        schd.systime++;

        if (schd.trigger_next <= schd.systime) {
            schd.last_event |= SCH_EVENT_CCR1;
            _BIC_SR_IRQ(LPM3_bits);
        }
    } else if (iv == HAL_SCH_IV_IFG) {
        // overflow
        HAL_SCH_CTL &= ~HAL_SCH_IFG;
    }
}

#endif
