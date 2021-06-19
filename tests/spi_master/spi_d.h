#ifndef __SPI_D_H__
#define __SPI_D_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_INTRA_BYTE_DELAY 275

void spid_read_frame(const uint16_t baseAddress, uint8_t * pBuffer, const uint16_t size);

void spid_send_frame(const uint16_t baseAddress, uint8_t * pBuffer, const uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
