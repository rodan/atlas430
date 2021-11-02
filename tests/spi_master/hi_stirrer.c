

#include <stdio.h>
#include <stdlib.h>
#include "eusci_b_spi.h"
//#include "helper.h"
#include "glue.h"
#include "spi_d.h"
#include "hi_stirrer.h"
#include "proj.h"

/* control register 0Eh/8Eh
bit7 EOSC   Enable Oscillator (1 if oscillator must be stopped when on battery)
bit6 BBSQW  Battery Backed Square Wave
bit5 CONV   Convert temperature (1 forces a conversion NOW)
bit4 RS2    Rate select - frequency of square wave output
bit3 RS1    Rate select
bit2 INTCN  Interrupt control (1 for use of the alarms and to disable square wave)
bit1 A2IE   Alarm2 interrupt enable (1 to enable)
bit0 A1IE   Alarm1 interrupt enable (1 to enable)
*/

void hist_init(const uint16_t baseAddress)
{
    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = 8000000;
    param.desiredSpiClock = 10000;
    param.msbFirst= EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase= EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    //param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(baseAddress, &param);
    EUSCI_B_SPI_enable(baseAddress);
}

void hist_port_init(void)
{
    HIST_CS_HIGH;
    // UCB1MOSI, UCB1MISO, UCB1CLK
    P5SEL0 |= (BIT0 | BIT1 | BIT2);
    P5SEL1 &= ~(BIT0 | BIT1 | BIT2);
}

void hist_read_reg(const uint16_t baseAddress, uint8_t addr, uint8_t *data, const uint8_t data_len)
{
    // basic sanity check
    if ((!data_len) || (addr > HIST_rERR_CHK)) {
        return;
    }

    HIST_CS_LOW;
    spid_send_frame(baseAddress, &addr, 1);
    spid_read_frame(baseAddress, data, data_len);
    HIST_CS_HIGH;

}

void hist_write_reg(const uint16_t baseAddress, uint8_t addr, uint8_t *data, const uint8_t data_len)
{
    uint8_t waddr = addr | HIST_bmWRITE;

    // basic sanity check
    if ((!data_len) || (addr > HIST_rPASSWD)) {
        return;
    }

    HIST_CS_LOW;
    spid_send_frame(baseAddress, &waddr, 1);
    spid_send_frame(baseAddress, data, data_len);
    HIST_CS_HIGH;
}


//////////////////////////////////////////////////////////////////////
// Function:            SPIComputeCheckSum()
// Overview:			Computes checksum to specified string
// PreCondition: 	    None
// Input:        	    pStrData - pointer to data string
//						yDataLength -  data string length
// Output:     		    checksum
// Notes:        		None
//////////////////////////////////////////////////////////////////////
uint8_t SPIComputeCheckSum(uint8_t* pStrData, uint8_t yDataLength)
{
	uint8_t yComputeLRC = 0;

	while(yDataLength--) {
		yComputeLRC += *pStrData++;
    }

	return ((uint8_t)(-((int8_t)yComputeLRC) + 1 ));
}

//////////////////////////////////////////////////////////////////////
// Function:            SPIVerifyCheckSum()
// Overview:			Reports if checksum is correct for an array
// PreCondition: 	    None
// Input:        	    pStrData - pointer to data string
//						yDataLength -  data string length
//                         must also contain the checksum as last byte
// Output:     		    returns 1 if checksum is correct
// Notes:        		None
//////////////////////////////////////////////////////////////////////
uint8_t SPIVerifyCheckSum(uint8_t* pStrData, uint8_t yDataLength)
{
	uint8_t yComputeLRC = 0;

	while(yDataLength--) {
		yComputeLRC += *pStrData++;
    }

    if (yComputeLRC == 1) {
        // correct checksum
        return 1;
    } else {
        return 0;
    }
}


