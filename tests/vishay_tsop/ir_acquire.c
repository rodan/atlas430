
#include <msp430.h>
#include <stdlib.h>
#include <inttypes.h>

#include "proj.h"
#include "driverlib.h"
#include "timer_a0.h"
#include "ir_acquire.h"

// sigrok-cli -d fx2lafw --config samplerate=100000 --channels D0,D1 --time 100 --triggers D0=f --protocol-decoders timing:data=D1

volatile uint16_t ir_acquire_deltas[IR_ACQUIRE_MAX_DELTAS];  // a vector containing deltas from the last edge
volatile uint16_t ir_acquire_sm_last_edge;      // at what timer_a0 tick was the last edge
volatile uint16_t ir_acquire_sm_edge_cntr;      // a counter for the number of edges in the current command
volatile uint16_t ir_acquire_sm_state;          // current state machine state
volatile uint8_t ir_acquire_last_level;         // if the last level was LOW or HIGH

int ir_acquire_get_acquisition(uint16_t **data, uint16_t *size)
{
    *data = (uint16_t *) ir_acquire_deltas;
    *size = ir_acquire_sm_edge_cntr;

    return EXIT_SUCCESS;
}

void ir_acquire_sm_set_state(const uint16_t state)
{
    ir_acquire_sm_state = state;
}

uint16_t ir_acquire_sm_get_state(void)
{
    return ir_acquire_sm_state;
}

void ir_acquire_start()
{
    ir_acquire_sm_state = IR_ACQUIRE_SM_START;

    // enable gpio interrupt for TSOP pin
    // IRQ triggers on falling edge
    IR_IES |= IR_TRIG;
    // Reset IRQ flags
    IR_IFG &= ~IR_TRIG;
    // Enable gpio interrupt
    IR_IE |= IR_TRIG;
}

int ir_acquire_alloc()
{

    return EXIT_SUCCESS;
}

int ir_acquire_free()
{

    return EXIT_SUCCESS;
}

void ir_acquire_sm(void)
{
    switch (ir_acquire_sm_state) {
        case IR_ACQUIRE_SM_START:
            // mark the CCR of the starting edge
            ir_acquire_sm_last_edge = TA0R;
            sig2_on;
            // disable the gpio IRQ
            IR_IE &= ~IR_TRIG;
            ir_acquire_sm_edge_cntr = 0;
            
            // mark the end of the acquiring function
            timer_a0_delay_noblk_ccr2(IR_ACQUIRE_MAX_INTERVAL);

            ir_acquire_last_level = IR_IN & IR_TRIG;
            ir_acquire_sm_state = IR_ACQUIRE_SM_ONGOING;

            // fallthrough
        case IR_ACQUIRE_SM_ONGOING:
            sig3_on;
            if ((IR_IN & IR_TRIG) != ir_acquire_last_level) {
                // the signal has flipped
                if (ir_acquire_sm_edge_cntr < IR_ACQUIRE_MAX_DELTAS - 2) {
                    // safe to save the current delta, no overflow
                    ir_acquire_deltas[ir_acquire_sm_edge_cntr] = TA0R - ir_acquire_sm_last_edge;
                    ir_acquire_sm_last_edge = TA0R;
                    ir_acquire_last_level = IR_IN & IR_TRIG;
                    ir_acquire_sm_edge_cntr++;
                } else {
                    // overflow would happen, so stop here
                    ir_acquire_sm_state = IR_ACQUIRE_SM_STOP;
                }
            }
            // come back via the timer IRQ
            timer_a0_delay_noblk_ccr1(2);
            sig3_off;
            break;
        case IR_ACQUIRE_SM_STOP:
            sig2_off;
            ir_acquire_sm_state = IR_ACQUIRE_SM_IDLE;
            break;
    }

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
__attribute__ ((interrupt(PORT1_VECTOR)))
void Port1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    if (P1IFG & IR_TRIG) {
        P1IFG &= ~IR_TRIG;
        // start the state machine
        ir_acquire_sm();
    }
}

