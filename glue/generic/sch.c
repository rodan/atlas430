
//   timer a2 handling
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/atlas430
//   license:         BSD

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "sch.h"

#include "sig.h"

static sch_descriptor schd;

uint8_t sch_init(void)
{
    __disable_interrupt();
    HAL_SCH_TAxCTL = TASSEL__ACLK + MC__CONTINOUS + TACLR;
    HAL_SCH_TAxEX0 = 0;
    __enable_interrupt();

    // ccr1 will count up to ~10ms
    HAL_SCH_TAxCCTL1 = 0;
    HAL_SCH_TAxCCR1 = HAL_SCH_TAxR + SCH_TICK;
    HAL_SCH_TAxCCTL1 = CCIE;

    schd.systime = 0;
    schd.trigger_schedule = 0;
    schd.trigger_next = 0;
    schd.last_event = SCH_EVENT_NONE;
    schd.systime = 0;

    return EXIT_SUCCESS;
}

// slot is between 0 and SCH_SLOTS_COUNT-1 inclusive
uint8_t sch_set_trigger_slot(const uint16_t slot, const uint32_t trigger, const uint8_t flag)
{
    if (slot > SCH_SLOTS_COUNT - 1) {
        return EXIT_FAILURE;
    }

    schd.trigger_slot[slot] = trigger;
    if (flag == SCH_EVENT_ENABLE) {
        schd.trigger_schedule |= (1UL << slot);

        if (schd.trigger_next > trigger) {
            schd.trigger_next = trigger;
        }
    } else {
        schd.trigger_schedule &= ~(1UL << slot);
    }

    return EXIT_SUCCESS;
}

uint8_t sch_get_trigger_slot(const uint16_t slot, uint32_t * trigger, uint8_t * flag)
{
    if (slot > SCH_SLOTS_COUNT - 1) {
        return EXIT_FAILURE;
    }

    *trigger = schd.trigger_slot[slot];
    if (schd.trigger_schedule & (1UL << slot)) {
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

    if (schd.trigger_schedule) {
        for (c = 0; c < SCH_SLOTS_COUNT; c++) {
            shift = 1UL << c;
            if (schd.trigger_schedule & shift) {
                // signal if event time was reached
                if (schd.trigger_slot[c] <= schd.systime) {
                    schd.trigger_schedule &= ~shift;
                    schd.last_event_schedule |= shift;
                } else
                    // prepare the next trigger
                if (schd.trigger_slot[c] < schedule_trigger_next) {
                    schedule_trigger_next = schd.trigger_slot[c];
                }
            }
        }
        schd.trigger_next = schedule_trigger_next;
    }
}

#if defined(SCH_USES_T0A) || defined(SCH_USES_T1A) || defined(SCH_USES_T2A)

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=HAL_SCH_TIMER_VECTOR
__interrupt void SCH_TIMER_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(HAL_SCH_TIMER_VECTOR))) SCH_TIMER_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = HAL_SCH_TAxIV;

    if (iv == HAL_SCH_TAxIV_TACCR1) {
        // timer used by timer_a1_delay_noblk_ccr1()
        // disable interrupt
        HAL_SCH_TAxCCTL1 &= ~CCIE;
        HAL_SCH_TAxCCTL1 = 0;
        HAL_SCH_TAxCCR1 = HAL_SCH_TAxR + SCH_TICK;
        HAL_SCH_TAxCCTL1 = CCIE;
        schd.systime++;

        if (schd.trigger_next <= schd.systime) {
            schd.last_event |= SCH_EVENT_CCR1;
            _BIC_SR_IRQ(LPM3_bits);
        }
    } else if (iv == HAL_SCH_TAxIV_TAxIFG) {
        // overflow
        HAL_SCH_TAxCTL &= ~TAIFG;
    }
}

#endif
