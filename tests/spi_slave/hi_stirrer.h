#ifndef __HI_STIRRER_H_
#define __HI_STIRRER_H_

#include <inttypes.h>
#include "config.h"

// registers
#define       HIST_rSTATUS  0x00
#define   HIST_rSTATUS_CHK  0x01
#define   HIST_rTARGET_RPM  0x02
#define   HIST_rTARGET_CHK  0x03
#define     HIST_rLIVE_RPM  0x04
#define     HIST_rLIVE_CHK  0x05
#define     HIST_rMODEL_ID  0x06
#define    HIST_rSERIAL_NO  0x07
#define      HIST_rVERSION  0x13
#define       HIST_rID_CHK  0x15
#define      HIST_rERR_CNT  0x16
#define      HIST_rERR_CHK  0x18
#define       HIST_rPASSWD  0x1f


// bitmasks
#define       HIST_bmWRITE  0x80

#define        HIST_bmERR0  0x80
#define        HIST_bmERR1  0x40
#define        HIST_bmERR2  0x20
#define        HIST_bmBUSY  0x10
#define         HIST_bmADJ  0x08
#define         HIST_bmCLR  0x04
#define       HIST_bmAPPLY  0x02
#define         HIST_bmRST  0x01

// constants
#define       HIST_bPASSWD  0xeb
#define     HIST_bFAKEDATA  0x00

// state machine
#define        HIST_smIDLE  0x00
#define        HIST_smREAD  0x01
#define       HIST_smWRITE  0x02

typedef struct {
    uint8_t status;
    uint8_t status_chk;
    uint8_t set_rpm;
    uint8_t set_rpm_chk;
    uint8_t live_rpm;
    uint8_t live_rpm_chk;
    uint8_t model_id;
    uint8_t serial_no[12];
    uint8_t version[2];
    uint8_t id_chk;
    uint16_t err_cnt;
    uint8_t err_cnt_chk;
    uint8_t reserved[6];
    uint8_t password;
} __attribute__((packed)) hi_stirrer_t;


#ifdef __cplusplus
extern "C" {
#endif

void hist_struct_init(hi_stirrer_t *st);

#ifdef __cplusplus
}
#endif

#endif
