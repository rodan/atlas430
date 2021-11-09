#ifndef __MSP430_HAL_H__
#define __MSP430_HAL_H__

/**
* \addtogroup HAL
* \brief initialize the gpio pins
* \author Petre Rodan
* 
* \{
**/

/**
* \file
* \brief Include file for \ref HAL
* \author Petre Rodan
**/

///< flags to be used as gpio_flags in msp430_hal_init()
#define HAL_GPIO_DIR_FLOATING  0    ///< set all ports as gpio floating
#define   HAL_GPIO_DIR_OUTPUT  0x1  ///< set all ports as gpio output
#define      HAL_GPIO_OUT_LOW  0    ///< set all ports low (in case HAL_GPIO_OUTPUT is also set)
#define     HAL_GPIO_OUT_HIGH  0x2  ///< set all ports high (in case HAL_GPIO_OUTPUT is also set)

#ifdef __cplusplus
extern "C" {
#endif

/** initializes all gpio pins with a given PxDIR, PxOUT
    @param gpio_flags sets the port direction and output with the following flags: HAL_GPIO_DIR_FLOATING, HAL_GPIO_DIR_OUTPUT, HAL_GPIO_OUT_LOW, HAL_GPIO_OUT_HIGH
    @return void
*/
void msp430_hal_init(const uint8_t gpio_flags);

#ifdef __cplusplus
}
#endif

#endif
///\}
