#ifndef __IR_SEND_H__
#define __IR_SEND_H__

#include "ir_common.h"

// tsop pin
#define                  IR_IES  P1IES
#define                  IR_IFG  P1IFG
#define                   IR_IE  P1IE
#define                   IR_IN  P1IN
#define                 IR_TRIG  BIT2

// state machine states

#define       IR_SEND_SM_IDLE  0x00
#define      IR_SEND_SM_START  0x01
#define    IR_SEND_SM_ONGOING  0x02
#define       IR_SEND_SM_STOP  0x04

#ifdef __cplusplus
extern "C" {
#endif

void ir_send_start(struct ir_tome *cmd);
void ir_send_sm(void);
uint16_t ir_send_sm_get_state(void);

#ifdef __cplusplus
}
#endif

#endif
