
//   timer a0 handling
//   CCR1 is used for timer_a0_delay_noblk_ccr1()
//   CCR2 is used for timer_a0_delay_noblk_ccr2()
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/
//   license:         BSD

#include "timer_a0.h"

volatile uint8_t timer_a0_last_event;
volatile uint16_t timer_a0_ovf;

void timer_a0_init(void)
{

    // ID__4 & TAIDEX_2
    // 1 tick is TA0_DIVIDER / MCLK == 12/8000000 = 1.5 us
    // overflow happens after .0015*65535 = 98.30 ms

    // ID__8 & TAIDEX_3
    // 1 tick is TA0_DIVIDER / MCLK == 32/8000000 = 4 us
    // overflow happens after .004*65535 = 262.14 ms

    __disable_interrupt();
    timer_a0_ovf = 0;
    TA0CTL = TASSEL__SMCLK + MC__CONTINOUS + TACLR + ID__8 + TAIE; // divide SMCLK by 8
    TA0EX0 = TAIDEX_3; // further divide SMCLK by 4
    __enable_interrupt();
}

uint8_t timer_a0_get_event(void)
{
    return timer_a0_last_event;
}

void timer_a0_rst_event(void)
{
    timer_a0_last_event = TIMER_A0_EVENT_NONE;
}

void timer_a0_delay_noblk_ccr1(uint16_t ticks)
{
    TA0CCTL1 &= ~CCIE;
    TA0CCTL1 = 0;
    TA0CCR1 = TA0R + ticks;
    TA0CCTL1 = CCIE;
}

void timer_a0_delay_ccr2(uint16_t ticks)
{
    TA0CCTL2 &= ~CCIE;
    TA0CCTL2 = 0;
    TA0CCR2 = TA0R + ticks;
    TA0CCTL2 = CCIE;

    timer_a0_last_event &= ~TIMER_A0_EVENT_CCR2;
    while (1) {
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
        if (timer_a0_last_event & TIMER_A0_EVENT_CCR2)
            break;
    }
    TA0CCTL2 &= ~CCIE;
    timer_a0_last_event &= ~TIMER_A0_EVENT_CCR2;
}

__attribute__ ((interrupt(TIMER0_A1_VECTOR)))
void timer0_A1_ISR(void)
{
    uint16_t iv = TA0IV;
    if (iv == TAIV__TACCR1) {
        // timer used by timer_a0_delay_noblk_ccr1()
        // disable interrupt
        TA0CCTL1 &= ~CCIE;
        TA0CCTL1 = 0;
        timer_a0_last_event |= TIMER_A0_EVENT_CCR1;
        _BIC_SR_IRQ(LPM3_bits);
    } else if (iv == TAIV__TACCR2) {
        // timer used by timer_a0_delay_noblk_ccr2()
        // disable interrupt
        TA0CCTL2 &= ~CCIE;
        TA0CCTL2 = 0;
        timer_a0_last_event |= TIMER_A0_EVENT_CCR2;
        _BIC_SR_IRQ(LPM3_bits);
    } else if (iv == TA0IV_TA0IFG) {
        TA0CTL &= ~TAIFG;
        timer_a0_ovf++;
        timer_a0_last_event |= TIMER_A0_EVENT_IFG;
        _BIC_SR_IRQ(LPM3_bits);
    }
}
