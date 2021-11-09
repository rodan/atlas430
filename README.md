
## reference libraries for msp430

This library provides a glue layer between the upstream [Texas Instruments mspdriverlib](https://www.ti.com/tool/MSPDRIVERLIB) and projects that need quick prototyping. On one hand it has a number of configurable compile-time defines that will set up the hardware abstraction layer allowing the user to focus on the higher level code and on the other hand it can be out-of-the-way and allow direct access to the registers or to the ti libs.


![Lib Logo](./doc/img/target_devboard.png)


```
 source:    https://github.com/rodan/reference_libs_msp430
 author:    Petre Rodan <2b4eda@subdimension.ro>
 license:   BSD
```

### components

* TI MSP430 DriverLib
* HAL glue for
  * system and auxiliary clocks
  * default port init
  * uart with optional interrupt driven tx
  * i2c with blocking implementation
  * i2c controlled by interrupts
  * bitbanged i2c
  * interrupt-based event handling
  * scheduling (to be added soon)
  * ring buffer
* drivers
  * *Honeywell High Accuracy Ceramic* (HSC) and *Standard Accuracy Ceramic* (SSC) piezoresistive silicon pressure sensors
  * *Sensirion SHT 1x* humidity and temperature sensors
  * *Maxim DS3231* rtc IC
  * *Maxim DS3234* rtc IC
  * *Analog Devices AD7789* adc IC
  * *Cypress FM24xxx* i2c FRAM IC
  * *Texas Instruments TCA6408* IO expander 
* Makefile-based build, source code tag file and document generation, flashing, code static scan, automatic version control
* a Makefile driven collection of unit tests for different drivers
* generic scripts for checking the build environment, flashing uCs, version control, RTC initialization

### software requirements

the entire development is done on a Gentoo Linux using makefiles, vim and gdb.
while none of those are a requirement to making the library part of any msp430 project, it happens to be the beaten road. the library can be used in the following ways:

* via a special project makefile (in a linux environment). see [this Makefile](https://github.com/rodan/sigdup/blob/master/firmware/Makefile) for the perfect example. **REFLIB_ROOT** defines the path to where this reference library has been cloned, **TARGET** represents the target microcontroller and [config.h](https://github.com/rodan/sigdup/blob/master/firmware/config.h) will be automatically expanded into compilation macros (-DFOO arguments to be sent to gcc). the included [Makefile.env](https://github.com/rodan/reference_libs_msp430/blob/master/Makefile.env) contains the paths for the excelent TI msp430 toolchain which is a requirement in this scenario. a script for checking the build environment is [provided](./tools/check_setup.sh). it can be run on a non-priviledged account and will provide pointers of what packages are needed for building using this library.

* via *Code Composer Studio for Linux*. import [this project](https://github.com/rodan/sigdup/tree/master/firmware) as an example. the reference library is expected to be symlinked or cloned into */opt/reference_libs_msp430/* while the compilation macros and the memory model are baked into the project's xml files - one needs to tweak them via **project properties**.
compile and debug as usual via what CCS offers.

* via *Code Composer Studio for Windows*, theoretically like above. I have never tested this. I might do it someday, maybe.

### support

tests of the library are performed on the following target devices:

microcontroller | family | functionality
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

### references 

the following projects are already using this library:

project | microcontroller | short description
------- | --------------- | -----------------
[sigdup](https://github.com/rodan/sigdup) | msp430fr5994 | software that takes PulseView digital signal captures as input and re-plays them
[solar-charger rev4.1](https://github.com/rodan/solar-charger) | msp430f5510 | off-the-grid 1 cell lipo battery charger for very low efficiency 15-36V photovoltaic cells




