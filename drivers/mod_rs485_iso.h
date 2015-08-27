#ifndef __MOD_RS485_ISO_H__
#define __MOD_RS485_ISO_H__

#include "proj.h"

#define SET_CLC    0x30
#define SET_ADDR   0xB0

uint8_t mod_rs485_iso_tx_cmd(const uint8_t addr, const uint8_t command, const uint8_t arg);

#endif
