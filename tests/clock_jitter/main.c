
#include <msp430.h>
#include <stdint.h>
#include "driverlib.h"

#define       ACLK_FREQ  32768
#define  CLK_LFXT_DRIVE  CS_LFXT_DRIVE_0
#define      SMCLK_FREQ  16000000
#define  CLK_HFXT_DRIVE  CS_HFXT_DRIVE_16MHZ_24MHZ
#define    PMM_CORE_LVL  PMM_CORE_LEVEL_2

uint8_t sig = 0;
uint16_t ccr = 0;

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // setup gpio
    P3DIR = 0xff;               // all P3.x are output
    PJSEL0 |= BIT4 | BIT5;      // XT1
    PJSEL0 |= BIT6 | BIT7;      // XT2
    PM5CTL0 &= ~LOCKLPM5;       // turn on gpio

    // setup clock
    CS_setExternalClockSource(ACLK_FREQ, SMCLK_FREQ);
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_turnOnLFXT(CLK_LFXT_DRIVE);
    CS_turnOnHFXT(CLK_HFXT_DRIVE);

    // setup timer
    // 0.0625us per clock tick, overflow at 4.096ms
    TA1CTL = TASSEL__SMCLK + MC__CONTINOUS + TACLR + ID__1;     // divide by 1
    TA1EX0 = TAIDEX_0;          // further divide by 1
    TA1CCR1 = 0;
    TA1CTL |= TACLR;
    TA1CCTL1 = CCIE;

    while (1) {
        _BIS_SR(LPM0_bits + GIE);
    }

}

__attribute__((interrupt(TIMER1_A1_VECTOR)))
void timer1_A1_ISR(void)
{
    uint16_t iv = TA1IV;

    if (iv == TAIV__TACCR1) {
        P3OUT = sig;
        TA1CCTL1 = 0;
        TA1CCR1 = ccr;
        TA1CCTL1 = CCIE;

        sig++;
        //ccr += 160;   // 50kHz on P3.0
        //ccr += 180;   // 45kHz on P3.0
        ccr += 200;             // 40kHz on P3.0
        //ccr += 320;   // 25kHz on P3.0
    } else if (iv == TA1IV_TA1IFG) {
        TA1CTL &= ~TAIFG;
    }
}

