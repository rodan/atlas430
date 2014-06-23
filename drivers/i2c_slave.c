
#include <stdint.h>

#include "proj.h"
#include "i2c_slave.h"
#include "i2c_internal.h"

#ifdef I2C_SLAVE

__attribute__ ((interrupt(I2C_ISR_VECTOR)))
void USCI_BX_ISR(void)
{
    volatile uint8_t devnull;
    enum i2c_tevent ev = 0;

    switch (I2C_IV) {
    case 0:                     // Vector  0: No interrupts
        break;
    case 2:                     // Vector  2: ALIFG
        break;
    case 4:                     // Vector  4: NACKIFG
        break;
    case 6:                    // Vector  6: STTIFG
        I2C_IFG &= ~UCSTTIFG;   // Clear start condition int flag
        // feed data from this address onward
        i2c_slave_tx_data = i2c_slave_tx_data_start_addr;
        // always receive data to this addr
        i2c_slave_rx_data = i2c_slave_rx_data_start_addr;
        i2c_rx_ctr = 0;
        break;
    case 8:                    // Vector  8: STPIFG
        I2C_IFG &= ~UCSTPIFG;   // Clear stop condition int flag
        if ( i2c_rx_rdy && i2c_rx_ctr) {
            i2c_rx_rdy = 0;
            ev |= I2C_EV_RX;
        }
        _BIC_SR_IRQ(LPM3_bits);
        break;
    case 10:                   // Vector 10: RXIFG
        if ( i2c_rx_rdy && (i2c_rx_ctr < I2C_RX_BUFF_LEN)) {
            *i2c_slave_rx_data++ = I2C_RXBUF;
            i2c_rx_ctr++;
        } else {
            // RXBUF needs to be read or i2c will get stuck
            devnull = I2C_RXBUF;
            devnull++; // silence compilation err
        }
        break;
    case 12:                   // Vector 12: TXIFG
        I2C_TXBUF = *i2c_slave_tx_data++;  // Transmit data at address PTxData
        break;
    default:
        break;
    }
    i2c_last_event |= ev;
}

void i2c_slave_init()
{
    I2C_CTL0 = UCMODE_3 + UCSYNC;       // I2C Slave, synchronous mode
    I2C_CTL1 |= UCSWRST;        // Enable SW reset
    I2C_OA = I2C_SLAVE_ADDR;    // Own Address
    I2C_CTL1 &= ~UCSWRST;       // Clear SW reset, resume operation
    I2C_IE |= UCTXIE + UCRXIE + UCSTPIE + UCSTTIE;
}

void i2c_slave_uninit(void)
{
    I2C_CTL0 |= UCSWRST;
}

#endif
