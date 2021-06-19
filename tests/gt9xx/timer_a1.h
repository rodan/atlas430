#ifndef __TIMER_A1_H__
#define __TIMER_A1_H__

#include "proj.h"

//#define  _10ms  6666 // 65535*0.01/(12/8000000*65535) aka 0.01/(12/8000000)
#define  _100us 25
#define  _1ms   250
#define  _4ms   1000 // 0.004/(32/8000000)
#define  _10ms  2500 // 65535*0.01/(32/8000000*65535) aka 0.01/(32/8000000)
#define  _20ms  5000
#define  _100ms  25000
#define  _200ms  50000
#define  _260ms  65000

#define TIMER_A1_EVENT_NONE  0
#define TIMER_A1_EVENT_CCR1  0x1
#define TIMER_A1_EVENT_CCR2  0x2
#define  TIMER_A1_EVENT_IFG  0x4

void timer_a1_init(void);
void timer_a1_halt(void);
void timer_a1_delay_noblk_ccr1(uint16_t ticks);
void timer_a1_delay_ccr2(uint16_t ticks);

void timer_a1_delay_noblk_ccr1_disable(void);
uint8_t timer_a1_get_event(void);
void timer_a1_rst_event(void);

#endif
