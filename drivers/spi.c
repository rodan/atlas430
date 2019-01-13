#include "proj.h"
#include "spi.h"

#define SPI_SIMO        BIT1
//#define SPI_SOMI        BIT2
#define SPI_CLK         BIT3
#define SPI_SEL         P4SEL
#define SPI_DIR         P4DIR
#define SPI_REN         P4REN
#define SPI_OUT         P4OUT

void spi_init(void)
{

#ifdef SPI_SOMI
    SPI_SEL |= SPI_CLK | SPI_SOMI | SPI_SIMO;
    SPI_REN |= SPI_SOMI | SPI_SIMO;
    SPI_OUT |= SPI_SOMI | SPI_SIMO;
#else
    SPI_SEL |= SPI_CLK | SPI_SIMO;
    SPI_REN |= SPI_SIMO;
    SPI_OUT |= SPI_SIMO;
#endif

    //Initialize USCI_B1 for SPI Master operation
    UCB1CTL1 |= UCSWRST;        //Put state machine in reset
    //UCB1CTL0 = UCCKPH | UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC;      //3-pin, 8-bit SPI master
    UCB1CTL0 = UCMSB | UCMST | UCMODE_0 | UCSYNC;      //3-pin, 8-bit SPI master
    //Clock polarity select - The inactive state is high
    //MSB first
    UCB1CTL1 = UCSWRST | UCSSEL_2;      //Use SMCLK, keep RESET
    UCB1BR0 = 16;             //Initial SPI clock
    //UCB1BR0 = 8;                //Initial SPI clock
    UCB1BR1 = 0;                //f_UCxCLK = SMCLK/63
    UCB1CTL1 &= ~UCSWRST;       //Release USCI state machine
    UCB1IFG &= ~UCRXIFG;
}

void spi_end(void)
{

#ifdef SPI_SOMI
    SPI_SEL &= ~(SPI_CLK | SPI_SOMI | SPI_SIMO);
    SPI_DIR &= ~(SPI_CLK | SPI_SIMO | SPI_SOMI);
    SPI_REN &= ~(SPI_SOMI | SPI_SIMO);
#else
    SPI_SEL &= ~(SPI_CLK | SPI_SIMO);
    SPI_DIR &= ~(SPI_CLK | SPI_SIMO);
    SPI_REN &= ~SPI_SIMO;
#endif
}

void spi_fast_mode(void)
{
    UCB1CTL1 |= UCSWRST;        //Put state machine in reset
    UCB1BR0 = 2;                //f_UCxCLK = 25MHz/2 = 12.5MHz
    UCB1BR1 = 0;
    UCB1CTL1 &= ~UCSWRST;       //Release USCI state machine
}

void spi_read_frame(uint8_t * pBuffer, uint16_t size)
{
    uint16_t gie = _get_SR_register() & GIE;   //Store current GIE state

    __disable_interrupt();      //Make this operation atomic

    UCB1IFG &= ~UCRXIFG;        //Ensure RXIFG is clear

    //Clock the actual data transfer and receive the bytes
    while (size--) {
        while (!(UCB1IFG & UCTXIFG)) ;  //Wait while not ready for TX
        UCB1TXBUF = 0xff;       //Write dummy byte
        while (!(UCB1IFG & UCRXIFG)) ;  //Wait for RX buffer (full)
        *pBuffer++ = UCB1RXBUF;
    }

    _bis_SR_register(gie);          //Restore original GIE state
}

void spi_send_frame(uint8_t * pBuffer, uint16_t size)
{
    uint16_t gie = _get_SR_register() & GIE;   //Store current GIE state

    __disable_interrupt();      //Make this operation atomic

    //Clock the actual data transfer and send the bytes. Note that we
    //intentionally not read out the receive buffer during frame transmission
    //in order to optimize transfer speed, however we need to take care of the
    //resulting overrun condition.
    while (size--) {
        while (!(UCB1IFG & UCTXIFG)) ;  //Wait while not ready for TX
        UCB1TXBUF = *pBuffer++; //Write byte
    }
    while (UCB1STAT & UCBUSY) ; //Wait for all TX/RX to finish

    UCB1RXBUF;                  //Dummy read to empty RX buffer
    //and clear any overrun conditions

    _bis_SR_register(gie);          //Restore original GIE state
}

