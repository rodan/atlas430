
### testing

tests of the library are performed on the following target devices:

microcontroller | devboard                                                                                | family          |clock | uart | i2c  | spi  | scheduler
--------------- | --------------------------------------------------------------------------------------- | --------------- | ---- | ---- | ---- | ---- | ---------
CC430F5137      | [Olimex MSP430-CCRF](https://www.olimex.com/Products/MSP430/Starter/MSP430-CCRF)        | MSP430F5xx_6xx  | pass | pass | pass | pass | pass
MSP430F5510     | [Olimex MSP430-5510STK](https://www.olimex.com/Products/MSP430/Starter/MSP430-5510STK/) | MSP430F5xx_6xx  | pass | pass | pass | pass | -
MSP430F5529     | [TI MSP-EXP430F5529LP](https://www.ti.com/tool/MSP-EXP430F5529LP) (1)                   | MSP430F5xx_6xx  | pass | pass | pass | pass | -
MSP430FR2355    | [TI MSP-EXP430FR2355](https://www.ti.com/tool/MSP-EXP430FR2355)                         | MSP430FR2xx_4xx | pass | pass | pass | pass | pass
MSP430FR2433    | [TI MSP-EXP430FR2433](https://www.ti.com/tool/MSP-EXP430FR2433)                         | MSP430FR2xx_4xx | pass | pass | pass | pass | pass
MSP430FR2476    | [TI LP-MSP430FR2476](https://www.ti.com/tool/LP-MSP430FR2476)                           | MSP430FR2xx_4xx | pass | pass | pass | pass | pass
MSP430FR4133    | [TI MSP-EXP430FR4133](https://www.ti.com/tool/MSP-EXP430FR4133)                         | MSP430FR2xx_4xx | pass | fail | pass | pass | fail
MSP430FR5969    | [TI MSP-EXP430FR5969](https://www.ti.com/tool/MSP-EXP430FR5969)                         | MSP430FR5xx_6xx | pass | pass | pass | pass | -
MSP430FR5994    | [TI MSP-EXP430FR5994](https://www.ti.com/tool/MSP-EXP430FR5994) / [MSP-TS430PN80B](https://www.ti.com/tool/MSP-TS430PN80B) | MSP430FR5xx_6xx | pass | pass | pass | pass | pass
MSP430FR6989    | [TI MSP-EXP430FR6989](https://www.ti.com/tool/MSP-EXP430FR6989)                         | MSP430FR5xx_6xx | pass | fail | pass | pass | pass

(1) my MSP-EXP430F5529LP refuses to communicate with mspdebug, so support for this devboard is currently suspended

