/**
* \addtogroup MOD_I2C
* \{
**/

/**
* \file
* \brief Configuration include file for \ref MOD_I2C
* \author Alex Mykyta 
**/

#ifndef __I2C_CONFIG_H__
#define __I2C_CONFIG_H__

//==================================================================================================
/// \name Configuration
/// Configuration defines for the \ref MOD_I2C module
/// \{
//==================================================================================================

//  ===================================================
//  = NOTE: Actual ports must be configured manually! =
//  ===================================================

/// Select which USCI module to use
#define I2C_USE_DEV      1    ///< \hideinitializer
/**<    0 = USCIB0 \n
*         1 = USCIB1 \n
*         2 = USCIB2 \n
*         3 = USCIB3 \n
*         4 = EUSCIB0 \n
*         5 = EUSCIB1 \n
*         6 = EUSCIB2 \n
*         7 = EUSCIB3 \n
**/

#define I2C_USES_UCB1

/// Select which clock source to use
#define I2C_CLK_SRC     2    ///< \hideinitializer
/**<    0 = External \n
*        1 = ACLK    \n
*        2 = SMCLK
**/

/// Clock Division (0x0004 to 0xFFFF)
#define I2C_CLK_DIV     20 ///< \hideinitializer

/// optional eUSCI Control Word Register 1
#define I2C_CWR1        0   ///< \hideinitializer

#define I2C_BASE_ADDR   USCI_B1_BASE
#define I2C_ISR_VECTOR  USCI_B1_VECTOR

///\}

#endif
///\}
