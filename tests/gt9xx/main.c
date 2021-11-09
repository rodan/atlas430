
/*
    reference MSP430 library for an HI6000 EDGE probe

    tweak the baud rate in config.h
*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "proj.h"
#include "driverlib.h"
#include "glue.h"
#include "ui.h"
#include "timer_a0.h"
#include "timer_a1.h"
#include "i2c_config.h"
#include "gt9xx.h"

uint8_t coord_buff[100];
struct goodix_ts_data ts;
extern uint8_t gt9xx_debug_status[GT9XX_DEBUG_BUFFER_SZ];

void main_init(void)
{

    msp430_hal_init(HAL_GPIO_DIR_OUTPUT | HAL_GPIO_OUT_LOW);

    P5OUT = 0;
    P5DIR = 0xbf;
    P5REN = 0;
    P5IES = 0;
    P5IFG = 0;
    P5IE = 0;
    P5SEL0 = 0;
    P5SEL1 = 0;

    P6OUT = 0;
    P6DIR = 0xff;
    P6REN = 0;
    P6IES = 0;
    P6IFG = 0;
    P6IE = 0;
    P6SEL0 = 0;
    P6SEL1 = 0;

    P7OUT = 0;
    P7DIR = 0xFF;

    P8DIR = 0xFF;
    P8OUT = 0;

    PJOUT = 0;
    PJDIR = 0xFFFF;

    vcc_on;

    // port init
    P1DIR |= BIT0 + BIT3 + BIT4 + BIT5;

/*
    P5DIR &= ~BIT6;  // set as input
    P5OUT |= BIT6;   // pull-up resistor
    P5REN |= BIT6;   // select pull-up mode
    P5IES |= BIT6;   // IRQ triggers on falling edge
    P5IFG &= ~BIT6;  // reset IRQ flags
//    P5IE |= BIT6;    // enable irq
*/

    P6DIR &= ~GT9XX_IRQ;  // set as input
    P6REN &= ~GT9XX_IRQ;  // disable pullup/pulldown
    // falling edge triggering provides valid data every 15ms cycle
    P6IES |= GT9XX_IRQ;   // IRQ triggers on falling edge
    // rising edge triggering provides valid data every ~30ms
    //P6IES &= ~GT9XX_IRQ;  // IRQ triggers on rising edge
    P6IFG &= ~GT9XX_IRQ;  // reset IRQ flags

#ifdef HARDWARE_I2C
    P7SEL0 |= (BIT0 | BIT1);
    P7SEL1 &= ~(BIT0 | BIT1);
#endif

#ifdef USE_XT1
    PJSEL0 |= BIT4 | BIT5;
    CS_setExternalClockSource(32768, 0);
#endif

    // Set DCO Frequency to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    // Set DCO Frequency to 1MHz
    //CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_0);

    // configure MCLK, SMCLK to be sourced by DCOCLK
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

#ifdef USE_XT1
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_turnOnLFXT(CS_LFXT_DRIVE_3);
#endif

}

static void uart0_rx_irq(uint32_t msg)
{
    parse_user_input();
    uart0_set_eol();
}

static void timer_a0_irq(uint32_t msg)
{
    sig2_off;
    sig3_off;
    sig4_off;
}

uint8_t data[100];

void check_events(void)
{
    uint16_t msg = SYS_MSG_NULL;
    uint16_t ev;

    // gt9xx irq
    if (GT9XX_get_event() == GT9XX_EV_IRQ) {
        msg |= SYS_MSG_GT9XX_IRQ;
        GT9XX_rst_event();
    }

    // uart RX
    if (uart0_get_event() == UART0_EV_RX) {
        msg |= SYS_MSG_UART0_RX;
        uart0_rst_event();
    }

    ev = timer_a0_get_event();
    // timer_a0
    if (ev) {
        if ( ev == TIMER_A0_EVENT_CCR1) {
            msg |= SYS_MSG_TIMERA0_CRR1;
        } else if (ev == TIMER_A0_EVENT_CCR2) {
            msg |= SYS_MSG_TIMERA0_CRR2;
        }
        timer_a0_rst_event();
    }

    ev = timer_a1_get_event();
    // timer_a1
    if (ev) {
        if ( ev == TIMER_A1_EVENT_CCR1) {
            msg |= SYS_MSG_TIMERA1_CRR1;
        } else if (ev == TIMER_A1_EVENT_CCR2) {
            msg |= SYS_MSG_TIMERA1_CRR2;
        }
        timer_a1_rst_event();
    }
    
    if (msg != SYS_MSG_NULL) {
        eh_exec(msg);
    }
}

#ifdef DEBUG_STATUS
void touch_HL_handler(struct GT9XX_coord_t *coord)
{
    print_buf(gt9xx_debug_status, GT9XX_DEBUG_BUFFER_SZ);
}

#else
void touch_HL_handler(struct GT9XX_coord_t *coord)
{
    uint8_t i;
    char itoa_buf[18];
   
    uart0_print(" k");
    uart0_print(_utoa(itoa_buf, coord->key));
    uart0_print(" ");

    if (coord->key & 0x1) {
        sig2_on;
    } else {
        sig2_off;
    }
    if (coord->key & 0x2) {
        sig3_on;
    } else {
        sig3_off;
    }
    if (coord->key & 0x4) {
        sig4_on;
    } else {
        sig4_off;
    }

//    if (coord->key != 0) {
//        timer_a0_delay_noblk_ccr1(60000);
//    }

    for (i = 0; i < coord->count; i++) {
/*
        // detect if it's one of the buttons
        if (coord->point[i].x > 850) {
            if ((coord->point[i].y > 200) && (coord->point[i].y < 300)) {
                cb |= 0x1;
            } else if ((coord->point[i].y > 300) && (coord->point[i].y < 400)) {
                cb |= 0x2;
            } else if ((coord->point[i].y > 400) && (coord->point[i].y < 500)) {
                cb |= 0x4;
            }
        }
        if (cb & 0x1) {
            sig2_on;
        } else {
            sig2_off;
        }
        if (cb & 0x2) {
            sig3_on;
        } else {
            sig3_off;
        }
        if (cb & 0x4) {
            sig4_on;
        } else {
            sig4_off;
        }

        if (cb != 0) {
            // deassert the button-driven leds
            //timer_a0_delay_noblk_ccr1(11000);
        }

        if (rescale && (cb == 0)) {
            if ((coord->point[i].x > 70) && (coord->point[i].x < 758)) {
                rescale_x = (121 * (int32_t) coord->point[i].x - 10036) / 100;
                if (rescale_x > 800) {
                    rescale_x = 800;
                }
                if (rescale_x < 0) {
                    rescale_x = 0;
                }
                coord->point[i].x = (uint16_t) rescale_x;
            }
        }
*/
        uart0_print(" ");
        uart0_print(_utoa(itoa_buf, coord->point[i].trackId));
        uart0_print(" ");
        uart0_print(_utoa(itoa_buf, coord->point[i].x));
        uart0_print(" ");
        uart0_print(_utoa(itoa_buf, coord->point[i].y));
        uart0_print(" ");
        uart0_print(_utoa(itoa_buf, coord->point[i].area));
        uart0_print("   ");
    }
    uart0_print("\r\n");
}
#endif

int main(void)
{
    int16_t rv;
    char itoa_buf[18];

    sig0_on;
    // stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    main_init();
    timer_a0_init();
    timer_a1_init();
    uart0_port_init();
    uart0_init();

#ifdef UART0_RX_USES_RINGBUF
    uart0_set_rx_irq_handler(uart0_rx_ringbuf_handler);
#else
    uart0_set_rx_irq_handler(uart0_rx_simple_handler);
#endif

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

#ifdef HARDWARE_I2C 
    EUSCI_B_I2C_initMasterParam param = {0};

    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_BASE_ADDR, &param);

    #ifdef IRQ_I2C
        i2c_irq_init(EUSCI_BASE_ADDR);
    #endif
#endif

    ts.usci_base_addr = EUSCI_BASE_ADDR;
    ts.slave_addr = GT9XX_SA;

    rv = GT9XX_init(&ts);
    GT9XX_set_HLHandler(touch_HL_handler);

    uart0_print("GT9XX_init ret: ");
    uart0_print(_utoh(&itoa_buf[0], rv));
    uart0_print("\r\n");

    //timer_a1_delay_ccr2(_10ms); // wait 10 ms
    //P5OUT |= BIT2;
    //timer_a1_delay_ccr2(_10ms); // wait 10 ms

    eh_register(&uart0_rx_irq, SYS_MSG_UART0_RX);
    eh_register(&timer_a0_irq, SYS_MSG_TIMERA0_CRR1);

    display_menu();

    //P5IE |= BIT6;
    GT9XX_enable_irq();
    //P6IE |= GT9XX_IRQ;    // enable irq

    sig0_off;
    sig1_off;
    sig2_off;
    sig3_off;
    sig4_off;

    while (1) {
        // sleep
#ifdef LED_SYSTEM_STATES
        sig4_off;
#endif
        _BIS_SR(LPM3_bits + GIE);
        __no_operation();
#ifdef LED_SYSTEM_STATES
        sig4_on;
#endif
//#ifdef USE_WATCHDOG
//        WDTCTL = (WDTCTL & 0xff) | WDTPW | WDTCNTCL;
//#endif
        check_events();
        check_events();
        check_events();
        check_events();
        check_events();
    }
}

/*
// Port 5 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
#elif defined(__GNUC__)
__attribute__ ((interrupt(PORT5_VECTOR)))
void Port5_ISR(void)
#else
#error Compiler not supported!
#endif
{
    if (P5IFG & BIT6) {
        gt9xx_last_event = 1;
        P5IFG &= ~BIT6;
        LPM3_EXIT;
    }
}
*/
