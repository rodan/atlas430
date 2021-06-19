
//   timer a0 handling
//   CCR1 is used for timer_a1_delay_noblk_ccr1()
//   CCR2 is used for timer_a1_delay_ccr2() (blocking)
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/
//   license:         BSD

#include "timer_a1.h"

volatile uint8_t timer_a1_last_event;
volatile uint16_t timer_a1_ovf;

void timer_a1_init(void)
{

    // ID__4 & TAIDEX_2
    // 1 tick is TA1_DIVIDER / MCLK == 12/8000000 = 1.5 us
    // overflow happens after .0015*65535 = 98.30 ms

    // ID__8 & TAIDEX_3
    // 1 tick is TA1_DIVIDER / MCLK == 32/8000000 = 4 us
    // overflow happens after .004*65535 = 262.14 ms

    __disable_interrupt();
    timer_a1_ovf = 0;
    TA1CTL = TASSEL__SMCLK + MC__CONTINOUS + TACLR + ID__8; // divide SMCLK by 8
    TA1EX0 = TAIDEX_3; // further divide SMCLK by 4
    __enable_interrupt();
}

uint8_t timer_a1_get_event(void)
{
    return timer_a1_last_event;
}

void timer_a1_rst_event(void)
{
    timer_a1_last_event = TIMER_A1_EVENT_NONE;
}

void timer_a1_delay_noblk_ccr1(uint16_t ticks)
{
    TA1CCTL1 &= ~CCIE;
    TA1CCTL1 = 0;
    TA1CCR1 = TA1R + ticks;
    TA1CCTL1 = CCIE;
}

void timer_a1_delay_noblk_ccr1_disable(void)
{
    TA1CCTL1 &= ~CCIE;
    TA1CCTL1 = 0;   
}

void timer_a1_delay_ccr2(uint16_t ticks)
{
    TA1CCTL2 &= ~CCIE;
    TA1CCTL2 = 0;
    TA1CCR2 = TA1R + ticks;
    TA1CCTL2 = CCIE;

    timer_a1_last_event &= ~TIMER_A1_EVENT_CCR2;
    while (1) {
#ifdef LED_SYSTEM_STATES
        sig4_off;
#endif
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
        if (timer_a1_last_event & TIMER_A1_EVENT_CCR2) {
#ifdef LED_SYSTEM_STATES
            sig4_on;
#endif
            break;
            }
    }
    TA1CCTL2 &= ~CCIE;
    timer_a1_last_event &= ~TIMER_A1_EVENT_CCR2;
}

__attribute__ ((interrupt(TIMER1_A1_VECTOR)))
void timer1_A1_ISR(void)
{
    uint16_t iv = TA1IV;
#ifdef LED_SYSTEM_STATES
    sig2_on;
#endif
    if (iv == TAIV__TACCR1) {
        // timer used by timer_a1_delay_noblk_ccr1()
        // disable interrupt
        TA1CCTL1 &= ~CCIE;
        TA1CCTL1 = 0;
        timer_a1_last_event |= TIMER_A1_EVENT_CCR1;
        _BIC_SR_IRQ(LPM3_bits);
    } else if (iv == TAIV__TACCR2) {
        // timer used by timer_a1_delay_noblk_ccr2()
        // disable interrupt
        TA1CCTL2 &= ~CCIE;
        TA1CCTL2 = 0;
        timer_a1_last_event |= TIMER_A1_EVENT_CCR2;
        _BIC_SR_IRQ(LPM3_bits);
    } else if (iv == TA1IV_TA1IFG) {
        TA1CTL &= ~TAIFG;
        timer_a1_ovf++;
        //timer_a1_last_event |= TIMER_A1_EVENT_IFG;
        _BIC_SR_IRQ(LPM3_bits);
    }
#ifdef LED_SYSTEM_STATES
    sig2_off;
#endif
}
