#ifndef __IR_ACQUIRE_H__
#define __IR_ACQUIRE_H__

#include "ir_common.h"

// how many times an IR signal can rise and fall during one command
#define    IR_ACQUIRE_MAX_DELTAS  256

// maximum interval for one command (in milliseconds)
#define  IR_ACQUIRE_MAX_INTERVAL  _260ms

// tsop pin
#define                  IR_IES  P1IES
#define                  IR_IFG  P1IFG
#define                   IR_IE  P1IE
#define                   IR_IN  P1IN
#define                 IR_TRIG  BIT2

// state machine states

#define       IR_ACQUIRE_SM_IDLE  0x00
#define      IR_ACQUIRE_SM_START  0x01
#define    IR_ACQUIRE_SM_ONGOING  0x02
#define       IR_ACQUIRE_SM_STOP  0x04

#ifdef __cplusplus
extern "C" {
#endif

void ir_acquire_sm_set_state(const uint16_t state);
uint16_t ir_acquire_sm_get_state(void);
void ir_acquire_start(void);
void ir_acquire_sm(void);
int ir_acquire_get_acquisition(uint16_t **data, uint16_t *size);

#ifdef __cplusplus
}
#endif

#endif
