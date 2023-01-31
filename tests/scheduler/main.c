
#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"
#include "sig.h"

uart_descriptor bc; // backchannel uart interface

#define STATE_DEMO_0  0x0
#define STATE_DEMO_1  0x1
#define STATE_DEMO_2  0x2

uint8_t state_demo = STATE_DEMO_0;

static void uart_bcl_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart_set_eol(&bc);
}

void check_events(void)
{
    uint16_t ev = 0;
    uint16_t msg = SYS_MSG_NULL;

    // uart RX
    if (uart_get_event(&bc) & UART_EV_RX) {
        msg |= SYS_MSG_UART_BCL_RX;
        uart_rst_event(&bc);
    }
    // scheduler
    if (sch_get_event()) {
        msg |= SYS_MSG_SCHEDULER;
        sch_rst_event();
    }
    ev = sch_get_event_schedule();
    if (ev) {
        if (ev & (1 << SCHEDULE_DEMO_SM)) {
            msg |= SYS_MSG_DEMO_SM;
        }
        sch_rst_event_schedule();
    }

    eh_exec(msg);
}

static void scheduler_irq(uint32_t msg)
{
    sch_handler();
}

static void demo_sm(uint32_t msg)
{
    switch (state_demo) {
    case STATE_DEMO_0:
        sch_set_trigger_slot(SCHEDULE_DEMO_SM, systime() + 100, SCH_EVENT_ENABLE);
        state_demo = STATE_DEMO_1;
        sig0_switch;
        break;
    case STATE_DEMO_1:
        sch_set_trigger_slot(SCHEDULE_DEMO_SM, systime() + 200, SCH_EVENT_ENABLE);
        state_demo = STATE_DEMO_2;
        sig0_switch;
        break;
    case STATE_DEMO_2:
        sch_set_trigger_slot(SCHEDULE_DEMO_SM, systime() + 500, SCH_EVENT_ENABLE);
        state_demo = STATE_DEMO_0;
        sig0_switch;
        break;
    }
}

int main(void)
{
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);
#ifdef USE_SIG
    sig0_on;
#endif

    clock_pin_init();
    clock_init();

#if defined (__MSP430FR2433__) || defined (__MSP430FR2476__) || defined (__MSP430FR4133__) || defined (__MSP430FR5969__) || defined (__MSP430FR5994__)
    bc.baseAddress = EUSCI_A0_BASE;
#elif defined (__MSP430FR2355__) || defined (__MSP430FR6989__)
    bc.baseAddress = EUSCI_A1_BASE;
#elif defined (__CC430F5137__)
    bc.baseAddress = USCI_A0_BASE;
#elif defined (__MSP430F5510__) || defined (__MSP430F5529__)
    bc.baseAddress = USCI_A1_BASE;
#elif defined (__MSP430F5438__)
    bc.baseAddress = USCI_A3_BASE;
#endif
    bc.baudrate = BAUDRATE_57600;

#if defined __MSP430FR6989__
    P3SEL0 |= BIT4 | BIT5;
    P3SEL1 &= ~(BIT4 | BIT5);
#elif defined __MSP430FR2476__
    P1SEL0 |= BIT4 | BIT5;
    P1SEL1 &= ~(BIT4 | BIT5); 
#else
    uart_pin_init(&bc);
#endif
    uart_init(&bc);
#if defined UART_RX_USES_RINGBUF
    uart_set_rx_irq_handler(&bc, uart_rx_ringbuf_handler);
#else
    uart_set_rx_irq_handler(&bc, uart_rx_simple_handler);
#endif

    sch_init();

#ifdef USE_SIG
    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;
    //sig5_off;
#endif

    eh_init();
    eh_register(&uart_bcl_rx_irq, SYS_MSG_UART_BCL_RX);
    eh_register(&scheduler_irq, SYS_MSG_SCHEDULER);
    eh_register(&demo_sm, SYS_MSG_DEMO_SM);
    _BIS_SR(GIE);

    sch_set_trigger_slot(SCHEDULE_DEMO_SM, systime() + 5, SCH_EVENT_ENABLE);

    display_version();

    while (1) {
        // sleep
#ifdef USE_SIG
        sig4_off;
#endif
        _BIS_SR(LPM0_bits + GIE);
#ifdef USE_SIG
        sig4_on;
#endif
        __no_operation();
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
        check_events();
        check_events();
    }

}

