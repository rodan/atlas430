#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define        BAUDRATE_9600  0x1
#define       BAUDRATE_19200  0x2
#define       BAUDRATE_38400  0x3
#define       BAUDRATE_57600  0x4
#define      BAUDRATE_115200  0x5

#define        UART_RET_WAKE  1
#define       UART_RET_SLEEP  0

#define      UART0_RX_NO_ERR  0x0
#define   UART0_RX_WAKE_MAIN  0x1 // ringbuffer got a special value, wake up main loop
#define      UART0_RX_ERR_RB  0x2 // ringbuffer is full, cannot add new element

// values based on slau367p.pdf table 30-5 Typical Crystals and Baud Rates

// UCAxBRW.UCBRx - clock prescaler (16bit)
// UCAxMCTLW - 16bit modulation control word register (16bit)
//                     UCBRSx - second modulation stage select - 8bit
// UCBRFX - first modulation stage select - 4 bit | reserved - 3 bit | UCOS16 - 1 bit

#if SMCLK_FREQ == 1000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        
#define        BRW_9600_BAUD  
#define      MCTLW_9600_BAUD  

// err                        
#define       BRW_19200_BAUD  
#define     MCTLW_19200_BAUD  

// err                        
#define       BRW_38400_BAUD  
#define     MCTLW_38400_BAUD  

// err                        
#define       BRW_57600_BAUD  
#define     MCTLW_57600_BAUD  

// err                        
#define      BRW_115200_BAUD  
#define    MCTLW_115200_BAUD  


#elif SMCLK_FREQ == 4000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        
#define        BRW_9600_BAUD  
#define      MCTLW_9600_BAUD  

// err                        
#define       BRW_19200_BAUD  
#define     MCTLW_19200_BAUD  

// err                        
#define       BRW_38400_BAUD  
#define     MCTLW_38400_BAUD  

// err                        
#define       BRW_57600_BAUD  
#define     MCTLW_57600_BAUD  

// err                        
#define      BRW_115200_BAUD  
#define    MCTLW_115200_BAUD  


#elif SMCLK_FREQ == 8000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        
#define        BRW_9600_BAUD  
#define      MCTLW_9600_BAUD  

// err                        
#define       BRW_19200_BAUD  
#define     MCTLW_19200_BAUD  

// err                        
#define       BRW_38400_BAUD  
#define     MCTLW_38400_BAUD  

// err                        
#define       BRW_57600_BAUD  
#define     MCTLW_57600_BAUD  

// err                        
#define      BRW_115200_BAUD  
#define    MCTLW_115200_BAUD  


#elif SMCLK_FREQ == 16000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        
#define        BRW_9600_BAUD  
#define      MCTLW_9600_BAUD  

// err                        
#define       BRW_19200_BAUD  
#define     MCTLW_19200_BAUD  

// err                        
#define       BRW_38400_BAUD  
#define     MCTLW_38400_BAUD  

// err                        
#define       BRW_57600_BAUD  
#define     MCTLW_57600_BAUD  

// err                        
#define      BRW_115200_BAUD  
#define    MCTLW_115200_BAUD  

#elif SMCLK_FREQ == 24000000

#define             UC_CTLW0  UCSSEL__SMCLK

// err                        
#define        BRW_9600_BAUD  
#define      MCTLW_9600_BAUD  

// err                        
#define       BRW_19200_BAUD  
#define     MCTLW_19200_BAUD  

// err                        
#define       BRW_38400_BAUD  
#define     MCTLW_38400_BAUD  

// err                        
#define       BRW_57600_BAUD  
#define     MCTLW_57600_BAUD  

// err                        
#define      BRW_115200_BAUD  
#define    MCTLW_115200_BAUD  

#else
    #error SMCLK_FREQ is not defined or sourced from clock.h
#endif

#ifdef __cplusplus
}
#endif

#endif
