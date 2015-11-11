#ifndef __SIM900_H__
#define __SIM900_H__

#include "proj.h"
#include "fm24.h"

#define SIM900_DTR_HIGH         P1OUT |= BIT3
#define SIM900_DTR_LOW          P1OUT &= ~BIT3
#define SIM900_RTS_HIGH         P1OUT |= BIT6
#define SIM900_RTS_LOW          P1OUT &= ~BIT6
#define SIM900_PWRKEY_HIGH      P1OUT &= ~BIT7
#define SIM900_PWRKEY_LOW       P1OUT |= BIT7
#define SIM900_VBAT_ENABLE      P2OUT |= BIT0
#define SIM900_VBAT_DISABLE     P2OUT &= ~BIT0

#define SIM900_CTS              BIT5
#define SIM900_CTS_IN           P1IN & BIT5
#define SIM900_UCAIFG           UCA1IFG
#define SIM900_UCATXBUF         UCA1TXBUF

#define ERR_PIN_RDY             0x1
#define ERR_CALL_RDY            0x2
#define ERR_IMEI_UNKNOWN        0x4
#define ERR_SEND_FIX_GPRS       0x8
#define ERR_SEND_FIX_SMS        0x10
#define ERR_PARSE_SMS           0x20
#define ERR_SEND_SMS            0x40
#define ERR_GPRS_NO_IP_START    0x80
#define ERR_PARSE_CENG          0x100
#define ERR_RAM_WRITE           0x200
#define ERR_TASK_ADD            0x400

// depending on the error, the modem will not be reused 
// for at least these many seconds
#define BLACKOUT_PIN_RDY            300
#define BLACKOUT_CALL_RDY           300
#define BLACKOUT_IMEI_UNKNOWN       600
#define BLACKOUT_SEND_FIX_GPRS       60
#define BLACKOUT_GPRS_NO_IP_START   300

// state machine timeouts
#define SM_STEP_DELAY   _10ms * 2  // ~20ms
#define SM_R_DELAY      _10ms * 130 // ~1.3s

#define POST_VERSION            0x1

// states that can be reached by the low level state machine
typedef enum {
    SIM900_ON,
    SIM900_OFF,
    SIM900_PWRKEY_ACT,
    SIM900_VBAT_ON,
    SIM900_PRE_IDLE,
    SIM900_WAIT_FOR_RDY,
    SIM900_IDLE,
    SIM900_AT,
    SIM900_WAITREPLY,
    SIM900_VBAT_OFF,
    SIM900_SET1,
    SIM900_GET_IMEI,
    SIM900_PARSE_SMS,
    SIM900_DEL_SMS,
    SIM900_CLOSE_CMD,
    SIM900_TEXT_INPUT,
    SIM900_IP_INITIAL,
    SIM900_IP_START,
    SIM900_IP_GPRSACT,
    SIM900_IP_STATUS,
    SIM900_IP_CONNECT,
    SIM900_IP_CONNECT_OK,
    SIM900_TCP_START,
    SIM900_TCP_CLOSE,
    SIM900_IP_SEND,
    SIM900_IP_PUT,
    SIM900_IP_CLOSE,
    SIM900_IP_SHUT,
    SIM900_SEND_OK,
    SIM900_HTTP_REPLY,
    SIM900_SET_CENG,
} sim900_state_t;

// commands that are compatible with the low level state machine
typedef enum {
    CMD_NULL,
    CMD_ON,
    CMD_OFF,
    CMD_FIRST_PWRON,
    CMD_GET_READY,
    CMD_GET_IMEI,
    CMD_SEND_SMS,
    CMD_START_GPRS,
    CMD_POST_GPRS,
    CMD_CLOSE_GPRS,
    CMD_PARSE_SMS,
    CMD_PARSE_CENG
} sim900_cmd_t;

// highest level tasks for commanding a sim900
typedef enum {
    TASK_NULL,
    TASK_DEFAULT,
} sim900_task_t;

// discrete states for the high level state machine
typedef enum {
    SUBTASK_NULL,
    SUBTASK_ON,
    SUBTASK_WAIT_FOR_RDY,
    SUBTASK_GET_IMEI,
    SUBTASK_TX_GPRS,
    SUBTASK_START_GPRS,
    SUBTASK_CLOSE_GPRS,
    SUBTASK_HTTP_POST,
    SUBTASK_SEND_SMS,
    SUBTASK_PWROFF,
    SUBTASK_PARSE_SMS,
    SUBTASK_SWITCHER,
    SUBTASK_PARSE_CENG
} sim900_task_state_t;


// return values for subtasks in the high level sm
typedef enum {
    SUBTASK_NO_REPLY,
    SUBTASK_GET_IMEI_OK,
    SUBTASK_START_GPRS_OK,
    SUBTASK_CLOSE_GPRS_OK,
    SUBTASK_HTTP_POST_OK,
    SUBTASK_SEND_SMS_OK,
    SUBTASK_PARSE_SMS_OK,
    SUBTASK_PARSE_CENG_OK
} sim900_task_rv_t;

// command type
typedef enum {
    CMD_UNSOLICITED,
    CMD_SOLICITED,
    CMD_SOLICITED_GSN,
    CMD_IGNORE
} sim900_cmd_type_t;

// return codes
typedef enum {
    RC_NULL,
    RC_OK,
    RC_ERROR,
    RC_TMOUT,
    RC_TEXT_INPUT,
    RC_CMGS,
    RC_CMGL,
    RC_CMGR,
    RC_IMEI_RCVD,
    RC_STATE_IP_INITIAL,
    RC_STATE_IP_START,
    RC_STATE_IP_GPRSACT,
    RC_STATE_IP_STATUS,
    RC_STATE_IP_CONNECT,
    RC_STATE_IP_SHUT,
    RC_SEND_OK,
    RC_200_OK,
    RC_RCVD_OK,
    RC_CENG_RCVD
} sim900_rc_t;

typedef enum {
    TTY_NULL,
    TTY_RX_PENDING,
    TTY_RX_WAIT
} sim900_tty_t;

typedef enum {
    SMS_NULL,
    SMS_FIX,
    SMS_GENERIC_SETUP,
    SMS_GPRS_SETUP,
    SMS_GPS_TIMINGS,
    SMS_GPRS_TIMINGS,
    SMS_DEFAULTS,
    SMS_ERRORS,
    SMS_VREF,
    SMS_CODE_OK
} sim900_sms_subj_t;

#define TASK_MAX_RETRIES    3
#define TASK_QUEUE_SIZE     MAX_SEG + 12
#define SMS_QUEUE_SIZE      4

// modem status
#define RDY                 0x1
#define PIN_RDY             0x2
#define CALL_RDY            0x4
#define NEED_SYSTEM_REBOOT  0x8
#define GPRS_RDY            0x10

// modem flags
#define TX_FIX_RDY          0x1
#define TASK_IN_PROGRESS    0x2
#define BLACKOUT            0x4

// HTTP payload content
#define GEOFENCE_PRESENT    0x8
#define GPS_FIX_PRESENT     0x10

typedef struct {      // cell tower data
    uint16_t rxl;           // receive level
    uint16_t mcc;           // mobile country code
    uint16_t mnc;           // mobile network code
    uint16_t cellid;        // tower cell id
    uint16_t lac;           // location area code
} sim900_cell_t;

struct sim900_t {
    uint8_t checks;         // status register  - maybe remove?
    uint8_t rdy;            // ready status register {RDY, PIN_RDY ... }
    uint8_t flags;          // flags register {TASK_IN_PROGRESS, BLACKOUT ... }
    uint8_t trc;            // task retry counter [0 - TASK_MAX_RETRIES-1]
    uint8_t current_t;      // current task in the task queue [0 - TASK_QUEUE_SIZE-1]
    uint8_t last_t;         // number of entries in the task queue [0 - TASK_QUEUE_SIZE-1]
    sim900_task_state_t queue[TASK_QUEUE_SIZE]; // the task queue
    uint8_t current_s;      // current sms in the sms queue [0 - SMS_QUEUE_SIZE-1]
    uint8_t last_sms;       // number of entries in the sms queue
    sim900_sms_subj_t   sms_queue[SMS_QUEUE_SIZE];
    uint16_t err;           // error register {ERR_PIN_RDY, ERR_CALL_RDY ... }
    char imei[16];          // equipment IMEI
    char rcvd_sms_id[3];    // id of received SMS - as returned by the SIM card
    uint8_t rcvd_sms_id_len;    // length of received SMS string
    sim900_task_t task;     // high level sm {TASK_DEFAULT}
    sim900_task_state_t task_next_state;    // high sm state {SUBTASK_ON, SUBTASK_WAIT_FOR_RDY ... }
    sim900_task_rv_t task_rv;   // task state return value {SUBTASK_GET_IMEI_OK, SUBTASK_SEND_FIX_GPRS_OK ... }
    sim900_cmd_t cmd;       // low level command {CMD_ON, CMD_OFF ... }
    sim900_cmd_type_t cmd_type; // type of reply {CMD_UNSOLICITED, CMD_SOLICITED ... }
    sim900_rc_t  rc;        // return codes - obtained by parsing the reply {RC_OK, RC_ERROR ... }
    sim900_state_t next_state;  // low level sm state {SIM900_IP_START, SIM900_IP_GPRSACT ... }
    sim900_tty_t console;   // status of the console after a command is sent
    sim900_cell_t cell[4];  // tower cell data
};

struct sim900_t sim900;

void sim900_init(void);
void sim900_init_messagebus(void);
void sim900_start(void);
void sim900_halt(void);
void sim900_exec_default_task(void);

uint16_t sim900_tx_str(char *str, const uint16_t size);
uint16_t sim900_tx_cmd(char *str, const uint16_t size, const uint16_t reply_tmout);
uint8_t sim900_parse_rx(char *str, const uint16_t size);
uint8_t sim900_parse_sms(char *str, const uint16_t size);
uint8_t sim900_parse_ceng(char *str, const uint16_t size);
void extract_str(const char *haystack, const char *needle, char *str, uint8_t *len, const uint8_t maxlen);

uint8_t extract_dec(char *str, uint16_t *rv);
uint8_t extract_hex(char *str, uint16_t *rv);

uint8_t sim900_add_subtask(sim900_task_state_t subtask, sim900_sms_subj_t sms_subj);



#endif
