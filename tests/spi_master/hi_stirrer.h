#ifndef __HI_STIRRER_H_
#define __HI_STIRRER_H_

#include <inttypes.h>
#include "config.h"

// registers
#define         HIST_rSTATUS  0x00
#define     HIST_rSTATUS_CHK  0x01
#define           HIST_rMODE  0x02
#define       HIST_rMODE_CHK  0x03
#define     HIST_rTARGET_RPM  0x04
#define     HIST_rTARGET_CHK  0x06
#define       HIST_rLIVE_RPM  0x07
#define       HIST_rLIVE_CHK  0x09
#define       HIST_rMODEL_ID  0x0a
#define        HIST_rVERSION  0x0b
#define         HIST_rMV_CHK  0x0d
#define      HIST_rSERIAL_NO  0x0e
#define     HIST_rSERIAL_CHK  0x1a
#define        HIST_rERR_CNT  0x1b
#define        HIST_rERR_CHK  0x1d
#define         HIST_rPASSWD  0x1f

// register sizes
#define      HIST_rSTATUS_SZ  0x01
#define        HIST_rMODE_SZ  0x01
#define    HIST_rCHECKSUM_SZ  0x01
#define  HIST_rTARGET_RPM_SZ  0x02
#define    HIST_rLIVE_RPM_SZ  0x02
#define    HIST_rMODEL_ID_SZ  0x01
#define     HIST_rVERSION_SZ  0x02
#define   HIST_rSERIAL_NO_SZ  0x0c
#define     HIST_rERR_CNT_SZ  0x02
#define      HIST_rPASSWD_SZ  0x01

// bitmasks
#define         HIST_bmWRITE  0x80

#define          HIST_bmERR4  0x80 // eeprom read/write error
#define          HIST_bmERR2  0x40 // motor is blocked
#define          HIST_bmERR1  0x20 // overlapped write commands
#define       HIST_bmSTATE_R  0x08
#define           HIST_bmADJ  0x04
#define          HIST_bmBUSY  0x02
#define           HIST_bmCLR  0x02
#define           HIST_bmRST  0x01


// constants
#define         HIST_bPASSWD  0xeb
#define       HIST_bFAKEDATA  0x00 // byte returned if an out-of-bounds read is performed
#define   HIST_bLASTREADBYTE  0x1f
#define HIST_bMIN_PACKET_LEN  1
#define HIST_bMAX_PACKET_LEN  32
#define    HIST_defSERIAL_NO  "JYYWWNNNNRSC"

typedef struct {
    uint8_t status;
    uint8_t status_chk;
    uint8_t mode;
    uint8_t mode_chk;
    int16_t set_rpm;
    uint8_t set_rpm_chk;
    int16_t live_rpm;
    uint8_t live_rpm_chk;
    uint8_t model_id;
    uint8_t version[2];
    uint8_t mv_chk;
    uint8_t serial_no[12];
    uint8_t serial_no_chk;
    uint16_t err_cnt;
    uint8_t err_cnt_chk;
    uint8_t reserved;
    uint8_t password;
} hi_stirrer_t;

#ifdef __cplusplus
extern "C" {
#endif

void hist_init(const uint16_t baseAddress);
void hist_port_init(void);
void hist_read_reg(const uint16_t baseAddress, const uint8_t address, uint8_t *data, const uint8_t data_len);
void hist_write_reg(const uint16_t baseAddress, const uint8_t address, uint8_t *data, const uint8_t data_len);

uint8_t SPIComputeCheckSum(uint8_t* pStrData, uint8_t yDataLength);
uint8_t SPIVerifyCheckSum(uint8_t* pStrData, uint8_t yDataLength);


#ifdef __cplusplus
}
#endif

#endif
