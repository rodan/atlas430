

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eusci_b_spi.h"
//#include "helper.h"
#include "glue.h"
#include "hi_stirrer.h"
#include "proj.h"

#define HIST_defSERIAL_NO "JYYWWNNNNRSC"

void hist_struct_init(hi_stirrer_t *st)
{
    memset(st, 0, sizeof(hi_stirrer_t));
    memcpy((uint8_t *)&st->serial_no, HIST_defSERIAL_NO, 12);
    st->status = 0xee;
    st->status_chk = 0xcc;
    st->set_rpm = 0;
    st->set_rpm_chk = 0xcc;
    st->live_rpm = 0;
    st->live_rpm_chk = 0xcc;
    st->model_id = 0x11;
    st->version[0] = 0x01;
    st->version[1] = 0x23;
    st->id_chk = 0xcc;
    st->password = HIST_bPASSWD;
}


