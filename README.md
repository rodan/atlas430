
## reference libraries for msp430

This library provides a glue layer between the upstream [Texas Instruments mspdriverlib](https://www.ti.com/tool/MSPDRIVERLIB) and projects that need quick prototyping. On one hand it has a number of configurable compile-time defines that will set up the hardware abstraction layer allowing the user to focus on the higher level code and on the other hand it can be out-of-the-way and allow direct access to the registers or to the ti libs. feeling tripple blessed yet?

```
 source:    https://github.com/rodan/reference_libs_msp430
 author:    Petre Rodan <2b4eda@subdimension.ro>
 license:   BSD
```

### Components

* TI MSP430 DriverLib
* HAL glue for
  * system and auxiliary clocks
  * default port init
  * uart with optional interrupt driven tx
  * i2c with blocking implementation
  * i2c controlled by interrupts
  * bitbanged i2c
  * system messagebus
  * ring buffer
* drivers
  * *Honeywell High Accuracy Ceramic* (HSC) and *Standard Accuracy Ceramic* (SSC) piezoresistive silicon pressure sensors
  * *Sensirion SHT 1x* humidity and temperature sensors
  * *Maxim DS3231* rtc IC
  * *Maxim DS3234* rtc IC
  * *Analog Devices AD7789* adc IC
  * *Cypress FM24xxx* i2c FRAM IC
  * *Texas Instruments TCA6408* IO expander 
* Makefile-based build, document generation, flash, static scan, version control
* a Makefile driven collection of unit tests for different drivers
* generic scripts for checking the build environment, flashing uCs, version control, RTC initialization

### software requirements

the entire development is done in linux outside of the Code Composer Studio. once the library reaches a certain maturity I will try to make it more *CCS for Linux* friendly. if anyone is interested to make this also work with CCS for Windows, have at it. it's absolutely not something I want to sink my time into.

a script for checking the build environment is [provided](./tools/check_setup.sh). it can be run on a non-priviledged account and will provide pointers of what packages are needed for building using this library.

### support

tests of the library are performed on the following microcontrollers:

msp430 type | family | functionality
----------- | ------ | -------------
MSP430F5510 | MSP430F5xx_6xx | [x] clock [x] uart [x] i2c [x] spi
MSP430FR5994 | MSP430FR5xx_6xx | [x] clock [x] uart [x] i2c [x] spi
CC430F5137 | MSP430F5xx_6xx | [ ] clock [ ] uart [ ] i2c [ ] spi
MSP430F5529 | MSP430F5xx_6xx | [ ] clock [ ] uart [ ] i2c [ ] spi
MSP430F5438 | MSP430F5xx_6xx | [ ] clock [ ] uart [ ] i2c [ ] spi
MSP430FR6989 | MSP430FR5xx_6xx | [ ] clock [ ] uart [ ] i2c [ ] spi
MSP430FR4113 | MSP430FR2xx_4xx | [ ] clock [ ] uart [ ] i2c [ ] spi 
MSP430FR2476 | MSP430FR2xx_4xx | [ ] clock [ ] uart [ ] i2c [ ] spi
MSP430FR2433 | MSP430FR2xx_4xx | [ ] clock [ ] uart [ ] i2c [ ] spi
MSP430FR2355 | MSP430FR2xx_4xx | [ ] clock [ ] uart [ ] i2c [ ] spi

this list covers microcontrollers I have on hand. adding support for new ones is entirely possible and is mostly limited by devboard availability.

all R&D is done on a stable Gentoo Linux system with Mitto Systems GCC 9.3.1.1-based toolchain

