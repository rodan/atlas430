
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

sch_descriptor *sch_t0a_d;
sch_descriptor *sch_t1a_d;
sch_descriptor *sch_t2a_d;

uint8_t sch_init(sch_descriptor * schd)
{

    // configure TIMER subsystem based on baseAddress
    switch (schd->baseAddress) {
#if defined (TIMER_A0_BASE)
    case TIMER_A0_BASE:
        sch_t0a_d = schd;
        break;
#endif
#if defined (TIMER_A1_BASE)
    case TIMER_A1_BASE:
        sch_t1a_d = schd;
        break;
#endif
#if defined (TIMER_A2_BASE)
    case TIMER_A2_BASE:
        sch_t2a_d = schd;
        break;
#endif
    default:
        return EXIT_FAILURE;
        break;
    }

    __disable_interrupt();
    HWREG16(schd->baseAddress + OFS_TAxCTL) = TASSEL__ACLK + MC__CONTINOUS + TACLR;
    HWREG16(schd->baseAddress + OFS_TAxEX0) = 0;
    __enable_interrupt();

    // ccr1 will count up to 10ms
    HWREG16(schd->baseAddress + OFS_TAxCCTL1) = 0;
    HWREG16(schd->baseAddress + OFS_TAxCCR1) = HWREG16(schd->baseAddress + OFS_TAxR) + SCH_TICK;
    HWREG16(schd->baseAddress + OFS_TAxCCTL1) = CCIE;

    schd->systime = 0;
    schd->trigger_schedule = 0;
    schd->trigger_next = 0;
    schd->last_event = SCH_EVENT_NONE;
    schd->systime = 0;

    return EXIT_SUCCESS;
}

// slot is between 0 and SCH_SLOTS_COUNT-1 inclusive
uint8_t sch_set_trigger_slot(sch_descriptor * schd, const uint16_t slot, const uint32_t trigger,
                             const uint8_t flag)
{
    if (slot > SCH_SLOTS_COUNT - 1) {
        return EXIT_FAILURE;
    }

    schd->trigger_slot[slot] = trigger;
    if (flag == SCH_EVENT_ENABLE) {
        schd->trigger_schedule |= (1UL << slot);

        if (schd->trigger_next > trigger) {
            schd->trigger_next = trigger;
        }
    } else {
        schd->trigger_schedule &= ~(1UL << slot);
    }

    return EXIT_SUCCESS;
}

uint8_t sch_get_trigger_slot(const sch_descriptor * schd, const uint16_t slot, uint32_t * trigger,
                             uint8_t * flag)
{
    if (slot > SCH_SLOTS_COUNT - 1) {
        return EXIT_FAILURE;
    }

    *trigger = schd->trigger_slot[slot];
    if (schd->trigger_schedule & (1UL << slot)) {
        *flag = 1;
    } else {
        *flag = 0;
    }

    return EXIT_SUCCESS;
}

uint16_t sch_get_event(const sch_descriptor * schd)
{
    return schd->last_event;
}

void sch_rst_event(sch_descriptor * schd)
{
    schd->last_event = SCH_EVENT_NONE;
}

uint16_t sch_get_event_schedule(const sch_descriptor * schd)
{
    return schd->last_event_schedule;
}

void sch_rst_event_schedule(sch_descriptor * schd)
{
    schd->last_event_schedule = SCH_EVENT_NONE;
}

uint32_t sch_get_trigger_next(const sch_descriptor * schd)
{
    return schd->trigger_next;
}

uint32_t systime(const sch_descriptor * schd)
{
    return schd->systime;
}

void sch_handler(sch_descriptor * schd)
{
    uint16_t c;
    uint16_t shift;
    uint32_t schedule_trigger_next = 0;

    if (schd->trigger_schedule) {
        for (c = 0; c < SCH_SLOTS_COUNT; c++) {
            shift = 1UL << c;
            if (schd->trigger_schedule & shift) {
                // signal if event time was reached
                if (schd->trigger_slot[c] <= schd->systime) {
                    schd->trigger_schedule &= ~shift;
                    schd->last_event_schedule |= shift;
                } else
                    // prepare the next trigger
                if (schd->trigger_slot[c] < schedule_trigger_next) {
                    schedule_trigger_next = schd->trigger_slot[c];
                }
            }
        }
        schd->trigger_next = schedule_trigger_next;
    }
}

#ifdef SCH_USES_T0A

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=HAL_TIMER0A_VECTOR
__interrupt void TIMER0A_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(HAL_TIMER0A_VECTOR))) TIMER0A_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = TA0IV;

    if (iv == TAIV__TACCR1) {
        // timer used by timer_a1_delay_noblk_ccr1()
        // disable interrupt
        TA0CCTL1 &= ~CCIE;
        TA0CCTL1 = 0;
        TA0CCR1 = TA0R + SCH_TICK;
        TA0CCTL1 = CCIE;
        sch_systime++;

        if (sch_t0a_d->trigger_next <= sch_t0a_d->systime) {
            sch_t0a_d->last_event |= SCH_EVENT_CCR1;
            _BIC_SR_IRQ(LPM3_bits);
        }
    } else if (iv == TA0IV_TA0IFG) {
        // overflow
        TA0CTL &= ~TAIFG;
    }
}

#endif

#ifdef SCH_USES_T1A

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=HAL_TIMER1A_VECTOR
__interrupt void TIMER1A_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(HAL_TIMER1A_VECTOR))) TIMER1A_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = TA1IV;

    if (iv == HAL_TA1IV_TACCR1) {
        // timer used by timer_a1_delay_noblk_ccr1()
        // disable interrupt
        TA1CCTL1 &= ~CCIE;
        TA1CCTL1 = 0;
        TA1CCR1 = TA1R + SCH_TICK;
        TA1CCTL1 = CCIE;
        sch_t1a_d->systime++;

        if (sch_t1a_d->trigger_next <= sch_t1a_d->systime) {
            sch_t1a_d->last_event |= SCH_EVENT_CCR1;
            _BIC_SR_IRQ(LPM3_bits);
        }
    } else if (iv == TA1IV_TA1IFG) {
        // overflow
        TA1CTL &= ~TAIFG;
    }
}

#endif

#ifdef SCH_USES_T2A

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=HAL_TIMER2A_VECTOR
__interrupt void TIMER2A_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(HAL_TIMER2A_VECTOR))) TIMER2A_ISR(void)
#else
#error Compiler not supported!
#endif
{
    uint16_t iv = TA2IV;

    if (iv == TAIV__TACCR1) {
        // timer used by timer_a1_delay_noblk_ccr1()
        // disable interrupt
        TA2CCTL1 &= ~CCIE;
        TA2CCTL1 = 0;
        TA2CCR1 = TA2R + SCH_TICK;
        TA2CCTL1 = CCIE;
        sch_systime++;

        if (sch_t2a_d->trigger_next <= sch_t2a_d->systime) {
            sch_t2a_d->last_event |= SCH_EVENT_CCR1;
            _BIC_SR_IRQ(LPM3_bits);
        }
    } else if (iv == TA2IV_TA2IFG) {
        // overflow
        TA2CTL &= ~TAIFG;
    }
}

#endif
