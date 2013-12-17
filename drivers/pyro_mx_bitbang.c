
//  software bitbang of serial protocols
//  library that decodes Micronix Plus pocket pyrometer's synchronous serial debug output
//
//  see the enclosed micronix_pyrometer.decode file for details
//
//  author:          Petre Rodan <petre.rodan@simplex.ro>
//  available from:  https://github.com/rodan/
//  license:         GNU GPLv3

#include "pyro_mx_bitbang.h"
#include "sys_messagebus.h"
#include "timer_a0.h"
#include "calib.h"

static void parse_pyro_mx(enum sys_message msg)
{
    uint16_t temp_val = 0;
    uint8_t tmp[5];
    float temp_f;

    tmp[0] = pyro_mx_rx[0];
    tmp[1] = pyro_mx_rx[1];
    tmp[2] = pyro_mx_rx[2];
    tmp[3] = pyro_mx_rx[3];
    tmp[4] = pyro_mx_rx[4];

    if (((tmp[0] == 0x4c) || (tmp[0] == 0x66)) && (tmp[4] == 0x0d)) {
        temp_val |= (tmp[1] & 0x0f);
        temp_val <<= 8;
        temp_val |= tmp[2];
        temp_val <<= 4;
        temp_val |= (tmp[3] & 0xf0) >> 4;
        temp_f = ((PYR_MX_B * temp_val) + PYR_MX_A) * 10.0;
        if (tmp[0] == 0x4c) {
            pyro_mx.rem_temp_raw = temp_val;
            pyro_mx.rem_temp_float = temp_f;
            pyro_mx_last_event |= PYRO_MX_RDY;
        } else if (tmp[0] == 0x66) {
            pyro_mx.ref_temp_raw = temp_val;
            pyro_mx.ref_temp_float = temp_f;
        }
    }
}

uint8_t ssi_pyro_mx_init(void)
{
    pyro_mx_p = 0;
    // set both SCL and SDA pins as inputs
    SSI_PYRO_DIR &= ~(SSI_PYRO_SCL | SSI_PYRO_SDA);
    SSI_PYRO_OUT &= ~(SSI_PYRO_SCL | SSI_PYRO_SDA);
    // trigger on a Hi/Lo edge
    SSI_PYRO_IES |= SSI_PYRO_SCL;
    // clear IFG
    SSI_PYRO_IFG &= ~SSI_PYRO_SCL;
    // enable interrupt
    SSI_PYRO_IE |= SSI_PYRO_SCL;

    PYRO_MX_DIR &= ~PYRO_MX_ACT;
    PYRO_MX_OUT &= ~PYRO_MX_ACT;

    sys_messagebus_register(&parse_pyro_mx, SYS_MSG_PYRO_RX);

    return SSI_PYRO_OK;
}

__attribute__ ((interrupt(PORT1_VECTOR)))
void PORT1_ISR(void)
{
    uint16_t iv = P1IV;

    switch (iv) {
    case SSI_PYRO_SCL_IV:
        // clear IFG
        P1IFG &= ~SSI_PYRO_SCL;
        if (timer_a0_last_event & TIMER_A0_EVENT_CCR2) {
            // if no CLK signal is received for 1ms, consider the connection lost
            timer_a0_last_event &= ~TIMER_A0_EVENT_CCR2;
            pyro_mx_p = 0;
        }
        // set up TA0CCR2 for a 1ms trigger
        // this is kinda like timer_a0_delay_noblk(1000), but it's ISR friendly
        TA0CCTL2 &= ~CCIE;
        TA0CCR2 = TA0R + 32;    // 1000us/30.51 = 32
        TA0CCTL2 = 0;
        TA0CCTL2 = CCIE;

        if (pyro_mx_p == 0) {
            pyro_mx_rx[0] = 0;
            pyro_mx_rx[1] = 0;
            pyro_mx_rx[2] = 0;
            pyro_mx_rx[3] = 0;
            pyro_mx_rx[4] = 0;
        }
        if (SSI_PYRO_IN & SSI_PYRO_SDA) {
            pyro_mx_rx[pyro_mx_p / 8] |= 1 << (7 - (pyro_mx_p % 8));
        }
        pyro_mx_p++;
        // one sentence has 8*5 bits
        if (pyro_mx_p == 40) {
            pyro_mx_p = 0;
            pyro_mx_last_event |= PYRO_MX_RX;
            _BIC_SR_IRQ(LPM3_bits);
        }
    default:
        break;
    }
}
