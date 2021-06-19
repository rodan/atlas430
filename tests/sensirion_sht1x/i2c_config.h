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

// bitbang i2c
#define I2C_MASTER_DIR      P7DIR
#define I2C_MASTER_OUT      P7OUT
#define I2C_MASTER_IN       P7IN
#define I2C_MASTER_SCL      BIT1
#define I2C_MASTER_SDA      BIT0

#endif
///\}
