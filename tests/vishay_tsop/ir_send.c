
#include <msp430.h>
#include <stdlib.h>
#include <inttypes.h>

#include "proj.h"
#include "driverlib.h"
#include "timer_a0.h"
#include "ir_send.h"
#include "ir_common.h"

volatile uint16_t ir_send_sm_state;         // current state machine state
volatile uint16_t ir_send_sm_cntr;          // command delta counter
struct ir_tome *ir_current;

uint16_t ir_send_sm_get_state(void)
{
    return ir_send_sm_state;
}

void ir_send_start(struct ir_tome *cmd)
{
    ir_current = cmd;
    ir_send_sm_state = IR_SEND_SM_START;
    ir_send_sm();
}

void ir_send_sm(void)
{
    switch (ir_send_sm_state) {
        case IR_SEND_SM_START:
            sig2_on;
            ir_send_sm_cntr = 0;
            if (ir_current->delta_size) {
                ir_send_sm_state = IR_SEND_SM_ONGOING;
            } else {
                // FIXME checks
                //ir_send_sm_state = IR_SEND_SM_STOP;
                //timer_a0_delay_noblk_ccr2(10); // come back
            }
            // fallthrough
        case IR_SEND_SM_ONGOING:
            sig1_switch;
            timer_a0_delay_noblk_ccr2(ir_current->delta[ir_send_sm_cntr]);
            if (ir_send_sm_cntr == ir_current->delta_size) {
                ir_send_sm_state = IR_SEND_SM_STOP;
                timer_a0_delay_noblk_ccr2(10); // come back
            }
            ir_send_sm_cntr++;
            break;
        case IR_SEND_SM_STOP:
            //sig1_on;
            sig2_off;
            ir_send_sm_state = IR_SEND_SM_IDLE;
            break;
    }

}
