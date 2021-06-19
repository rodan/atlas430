/* crc.h */

#ifndef __ZCRC_H__
#define __ZCRC_H__

uint16_t crc16(const void *data, uint16_t length, uint16_t crc);
uint32_t crc32(const void *data, uint16_t length, uint32_t crc);

#ifdef ZMODEM_O_BYTESIZE_CRC32
void crc32bs_start(const uint32_t seed);
void crc32bs_upd(const uint8_t c);
uint32_t crc32bs_end(void);
#endif

#ifdef ZMODEM_O_BYTESIZE_CRC16
void crc16bs_start(const uint16_t seed);
void crc16bs_upd(const uint8_t c);
uint16_t crc16bs_end(void);
#endif

#endif
