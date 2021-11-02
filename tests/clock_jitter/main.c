


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
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// setup gpio
	P3DIR = 0xff;				// all P3.x are output
	PJSEL0 |= BIT4 | BIT5;		// XT1
	PJSEL0 |= BIT6 | BIT7;		// XT2
	PM5CTL0 &= ~LOCKLPM5;		// turn on gpio

	// setup clock
	CS_setExternalClockSource(ACLK_FREQ, SMCLK_FREQ);
	CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_turnOnLFXT(CLK_LFXT_DRIVE);
    CS_turnOnHFXT(CLK_HFXT_DRIVE);

	// setup timer
	// 0.0625us per clock tick, overflow at 4.096ms
    TA1CTL = TASSEL__SMCLK + MC__CONTINOUS + TACLR + ID__1; // divide by 1
    TA1EX0 = TAIDEX_0; // further divide by 1
    TA1CCR1 = 0;
    TA1CTL |= TACLR;
    TA1CCTL1 = CCIE;

    while (1) {
    	_BIS_SR(LPM0_bits + GIE);
    }

}

__attribute__ ((interrupt(TIMER1_A1_VECTOR)))
void timer1_A1_ISR(void)
{
    uint16_t iv = TA1IV;

    if (iv == TAIV__TACCR1) {
        P3OUT = sig;
        TA1CCTL1 = 0;
        TA1CCR1 = ccr;
        TA1CCTL1 = CCIE;

        sig++;
        //ccr += 160;	// 50kHz on P3.0
        //ccr += 180;	// 45kHz on P3.0
        ccr += 200;	// 40kHz on P3.0
        //ccr += 320;	// 25kHz on P3.0
    } else if (iv == TA1IV_TA1IFG) {
        TA1CTL &= ~TAIFG;
    }
}

#if 0
#include <msp430.h>
#include "proj.h"
#include "config.h"
#include "glue.h"
#include "timer_a0.h"

void main_init(void);

int main(void)
{
    main_init();

    while (1) {
        // sleep
        //_BIS_SR(LPM3_bits + GIE);
        __no_operation();
        __delay_cycles(8000000);
        sig1_switch;
    }
}

void main_init(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;

    // port init
    P1OUT = 0;
    P1DIR = 0xff;

    P2OUT = 0;
    P2DIR = 0xff;

    P3OUT = 0;
    P3DIR = 0xff;

    P4OUT = 0;
    P4DIR = 0xff;

    // P55 and P56 are buttons
    P5OUT = 0;
    P5DIR = 0x9f;
    // activate pullup
    P5OUT = 0x60;
    P5REN = 0x60;
    // IRQ triggers on the falling edge
    P5IES = 0x60;

    P6OUT = 0;
    P6DIR = 0xff;

    P7OUT = 0;
    P7DIR = 0xff;

    P8OUT = 0;
    P8DIR = 0xff;

    PJOUT = 0;
    PJDIR = 0xffff;
    sig0_on;

    clock_port_init();
    clock_init();

    // output SMCLK on P3.4
    P3OUT &= ~BIT4;
    P3DIR |= BIT4;
    P3SEL1 |= BIT4;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
}

#endif
