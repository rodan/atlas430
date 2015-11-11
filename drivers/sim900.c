
//  library for SIM900 - a Multi-Band WCDMA/GSM/GPRS/EDGE module solution
//  author:          Petre Rodan <petre.rodan@simplex.ro>
//  available from:  https://github.com/rodan/
//  license:         GNU GPLv3


#include <string.h>
#include <stdio.h>

#include "proj.h"
#include "sim900.h"
#include "timer_a0.h"
#include "uart1.h"
#include "sys_messagebus.h"
#include "flash.h"
#include "rtc.h"
#include "fm24.h"

#ifdef DEBUG_GPRS
#include "uart0.h"
#endif

uint8_t sm_c; // state machine internal counter

char eom[2] = { 0x1a, 0x0 };

uint32_t payload_size;
uint8_t payload_content_desc;   // 1 byte that describes what the payload contains

// high level state machine
// use this to send commands to sim900
static void sim900_tasks(enum sys_message msg)
{
    uint8_t i;

    switch (sim900.task) {
        case TASK_DEFAULT:
            switch (sim900.task_next_state) {
                case SUBTASK_ON:
                    sim900.flags |= TASK_IN_PROGRESS;
                    sim900.cmd = CMD_ON;
                    sim900.next_state = SIM900_VBAT_ON;
                    sim900.task_next_state = SUBTASK_WAIT_FOR_RDY;
                    timer_a0_delay_noblk_ccr2(SM_STEP_DELAY); // - signal the low level state machine
                    // no need for ccr1 timeout since this command does not receive any input from hw
                    sim900.trc = 0;
                break;
                case SUBTASK_WAIT_FOR_RDY:
                    if ((sim900.trc < TASK_MAX_RETRIES) && ((sim900.rdy & CALL_RDY) == 0)) {
                        sim900.trc++;
                        sim900.cmd = CMD_GET_READY;
                        sim900.next_state = SIM900_IDLE;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY); // - signal the low level sm
                        timer_a0_delay_noblk_ccr1(_75s);
                    } else if (sim900.rdy & CALL_RDY) {
                        sim900.trc = 0;
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    } else if (sim900.trc == TASK_MAX_RETRIES) {
                        if ((sim900.rdy & RDY) == 0) {
                            // we did not get the unsolicited 'RDY' message
                            // so probably the sim900 is not set up yet
                            // 2400bps is better detected by sim900's autobauding
                            uart1_init(2400);
                            sim900.cmd = CMD_FIRST_PWRON;
                            sim900.next_state = SIM900_IDLE;
                            timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                            // uC will be reset by the low level sm
                        } else if ((sim900.rdy & PIN_RDY) == 0) {
                            // '+CPIN: READY' was not received
                            sim900.err |= ERR_PIN_RDY;
                            // pin error or card not inserted
                            // either way go to sleep
                            sim900.task_next_state = SUBTASK_PWROFF;
                            timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                            // retry after at least this interval
                            sim900.flags |= BLACKOUT;
                            gprs_blackout_lift = rtca_time.sys + BLACKOUT_PIN_RDY;
                        } else if ((sim900.rdy & CALL_RDY) == 0) {
                            // 'Call Ready' was not received
                            sim900.err |= ERR_CALL_RDY;
                            sim900.task_next_state = SUBTASK_PWROFF;
                            timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                            sim900.flags |= BLACKOUT;
                            gprs_blackout_lift = rtca_time.sys + BLACKOUT_CALL_RDY;
                        }
                    }
                break;
                case SUBTASK_GET_IMEI:
                    if ((sim900.trc < TASK_MAX_RETRIES) && sim900.task_rv != SUBTASK_GET_IMEI_OK ) {
                        sim900.cmd = CMD_GET_IMEI;
                        sim900.next_state = SIM900_IDLE;
                        sim900.trc++;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                        timer_a0_delay_noblk_ccr1(_3sp); // ~3s
                    } else if (sim900.task_rv == SUBTASK_GET_IMEI_OK) {
                        sim900.trc = 0;
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    } else if (sim900.trc == TASK_MAX_RETRIES) {
                        sim900.err |= ERR_IMEI_UNKNOWN;
                        // this also happens when sim900 is unwilling/unable
                        // to receive commands, so just halt the chip
                        sim900.task_next_state = SUBTASK_PWROFF;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                        sim900.flags |= BLACKOUT;
                        gprs_blackout_lift = rtca_time.sys + BLACKOUT_IMEI_UNKNOWN;
                    }
                break;
                case SUBTASK_SWITCHER:
                    sim900.task_rv = SUBTASK_NO_REPLY;
                    if (sim900.current_t < sim900.last_t) {
                        sim900.task_next_state = sim900.queue[sim900.current_t];
                        sim900.current_t++;
                    } else {
                        sim900.task_next_state = SUBTASK_PWROFF;
                    }
                    sim900.trc = 0;
                    timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                break;
                case SUBTASK_TX_GPRS:
                    GPS_IRQ_DISABLE;
                    sim900_add_subtask(SUBTASK_START_GPRS, SMS_NULL);
                    m.seg[m.seg_num] = m.e;
                    for (i=0; i < m.seg_num; i++) {
                        sim900_add_subtask(SUBTASK_HTTP_POST, SMS_NULL);
                    }
                    sim900_add_subtask(SUBTASK_CLOSE_GPRS, SMS_NULL);
                    sim900.task_next_state = SUBTASK_SWITCHER;
                    timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                break;
                case SUBTASK_START_GPRS:
                    if ((sim900.trc < TASK_MAX_RETRIES) && sim900.task_rv != SUBTASK_START_GPRS_OK ) {
                        sim900.cmd = CMD_START_GPRS;
                        sim900.next_state = SIM900_IP_INITIAL;
                        sim900.trc++;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                        timer_a0_delay_noblk_ccr1(_30s);
                    } else if (sim900.task_rv == SUBTASK_START_GPRS_OK) {
                        sim900.trc = 0;
                        sim900.rdy |= GPRS_RDY;
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    } else {
                        //sim900.err |= ERR_SEND_FIX_GPRS;
                        // continue with the next task
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    }
                break;
                case SUBTASK_HTTP_POST:
                    if ((sim900.rdy & GPRS_RDY) && (sim900.trc < TASK_MAX_RETRIES) && sim900.task_rv != SUBTASK_HTTP_POST_OK ) {
                        sim900.cmd = CMD_POST_GPRS;
                        sim900.next_state = SIM900_TCP_START;
                        sim900.trc++;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                        timer_a0_delay_noblk_ccr1(_30s); // XXX
                    } else if (sim900.task_rv == SUBTASK_HTTP_POST_OK) {
                        sim900.trc = 0;
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    } else {
                        sim900.err |= ERR_SEND_FIX_GPRS;
                        // continue with the next task
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                        sim900.flags |= BLACKOUT;
                        gprs_blackout_lift = rtca_time.sys + BLACKOUT_SEND_FIX_GPRS;
                    }
                break;
                case SUBTASK_CLOSE_GPRS:
                    if ((sim900.rdy & GPRS_RDY) && (sim900.trc < TASK_MAX_RETRIES) && sim900.task_rv != SUBTASK_CLOSE_GPRS_OK ) {
                        sim900.cmd = CMD_CLOSE_GPRS;
                        sim900.trc++;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                        timer_a0_delay_noblk_ccr1(_30s);
                    } else if (sim900.task_rv == SUBTASK_CLOSE_GPRS_OK) {
                        sim900.trc = 0;
                        if (m.seg_num < 2) {
                            sim900.flags &= ~TX_FIX_RDY;
                        }
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    } else {
                        // continue with the next task
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    }
                break;
                case SUBTASK_PARSE_CENG:
                    if ((sim900.trc < TASK_MAX_RETRIES) && sim900.task_rv != SUBTASK_PARSE_CENG_OK ) {
                        sim900.cmd = CMD_PARSE_CENG;
                        sim900.next_state = SIM900_IDLE;
                        sim900.trc++;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                        timer_a0_delay_noblk_ccr1(_14s); // timeout in 14s+
                    } else if (sim900.task_rv == SUBTASK_PARSE_CENG_OK) {
                        sim900.trc = 0;
                        // create a new packet with tower cell data
                        store_pkt();
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    } else if (sim900.trc == TASK_MAX_RETRIES) {
                        sim900.err |= ERR_PARSE_CENG;
                        // continue with the next task
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    }
                break;
                case SUBTASK_PARSE_SMS:
                    if ((sim900.trc < TASK_MAX_RETRIES) && sim900.task_rv != SUBTASK_PARSE_SMS_OK ) {
                        sim900.cmd = CMD_PARSE_SMS;
                        sim900.next_state = SIM900_IDLE;
                        sim900.trc++;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                        timer_a0_delay_noblk_ccr1(_14s); // XXX
                    } else if (sim900.task_rv == SUBTASK_PARSE_SMS_OK) {
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    } else if (sim900.trc == TASK_MAX_RETRIES) {
                        sim900.err |= ERR_PARSE_SMS;
                        // continue with the next task
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    }
                break;
                case SUBTASK_SEND_SMS:
                    if ((sim900.trc < TASK_MAX_RETRIES) && sim900.task_rv != SUBTASK_SEND_SMS_OK ) {
                        sim900.cmd = CMD_SEND_SMS;
                        sim900.next_state = SIM900_IDLE;
                        sim900.trc++;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                        timer_a0_delay_noblk_ccr1(_14s); // timeout in 14s+
                    } else if (sim900.task_rv == SUBTASK_SEND_SMS_OK) {
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    } else if (sim900.trc == TASK_MAX_RETRIES) {
                        sim900.err |= ERR_SEND_SMS;
                        // continue with the next task
                        sim900.task_next_state = SUBTASK_SWITCHER;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
                    }
                break;
                case SUBTASK_PWROFF:
                    GPS_IRQ_ENABLE;
                    sim900.task = TASK_NULL;
                    sim900.cmd = CMD_OFF;
                    timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                break;
                default:
                break;
            }
        break;
        default:
        break;
    }
}


// low level state machine
static void sim900_state_machine(enum sys_message msg)
{
    uint32_t i;

    switch (sim900.cmd) {

        ///////////////////////////////////
        //
        // poweron sequence
        //

        case CMD_ON:
            switch (sim900.next_state) {
                case SIM900_VBAT_ON:
                    LED_ON;
                    SIM900_VBAT_ENABLE;
                    SIM900_PWRKEY_HIGH;
                    SIM900_RTS_HIGH;
                    SIM900_DTR_LOW;

                    sim900.checks = 0;
                    sim900.rdy = 0;
                    sim900.cmd_type = CMD_UNSOLICITED;

                    PMAPPWD = 0x02D52;
                    P4MAP4 = PM_UCA1TXD;
                    P4MAP5 = PM_UCA1RXD;
                    P4SEL |= 0x30;
                    PMAPPWD = 0;
                    P1DIR |= 0x48;
                    uart1_init(9600);

                    /*
                    if (CONF_MIN_INTERFERENCE) {
                        GPS_DISABLE;
                    }
                    */

                    sim900.next_state = SIM900_PWRKEY_ACT;
                    timer_a0_delay_noblk_ccr2(_500ms);
                break;
                case SIM900_PWRKEY_ACT:
                    LED_OFF;
                    SIM900_PWRKEY_LOW;
                    sim900.next_state = SIM900_ON;
                    timer_a0_delay_noblk_ccr2(_1200ms);
                break;
                case SIM900_ON:
                    LED_ON;
                    SIM900_PWRKEY_HIGH;
                    sim900.next_state = SIM900_PRE_IDLE;
                    timer_a0_delay_noblk_ccr2(_2s);
                break;
                case SIM900_PRE_IDLE:
                    LED_OFF;
                    SIM900_RTS_LOW;
                    sim900.cmd = CMD_NULL;
                    sim900.next_state = SIM900_IDLE;
                    timer_a0_delay_noblk_ccr1(SM_STEP_DELAY); // signal the high level sm
                break;
            }
        break;

        ///////////////////////////////////
        //
        // "stay a while and listen"
        //

        case CMD_GET_READY:
            switch (sim900.next_state) {
                case SIM900_IDLE:
                    sim900.next_state = SIM900_WAIT_FOR_RDY;
                    timer_a0_delay_noblk_ccr2(_14s); // ~14s
                break;
                case SIM900_WAIT_FOR_RDY:
                    sim900.cmd = CMD_NULL;
                    sim900.next_state = SIM900_IDLE;
                    if (sim900.rdy & CALL_RDY) {
                        // wait 1 minute in order to get more tower cell info
                        // and any possible SMSs
                        timer_a0_delay_noblk_ccr1(_60s);
                    }
                break;
            }
        break;


        ///////////////////////////////////
        //
        // get modem IMEI
        //

        case CMD_GET_IMEI:
            switch (sim900.next_state) {
                case SIM900_IDLE:
                    sim900.next_state = SIM900_GET_IMEI;
                    timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                    sim900_tx_cmd("AT+GSN\r", 7, REPLY_TMOUT);
                    sim900.cmd_type = CMD_SOLICITED_GSN;
                break;
                case SIM900_GET_IMEI:
                    if (sim900.rc == RC_IMEI_RCVD) {
                        sim900.next_state = SIM900_IDLE;
                        sim900.cmd = CMD_NULL;
                        sim900.task_rv = SUBTASK_GET_IMEI_OK;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY); // signal high level sm
                    }
                break;
            }
        break;

        ///////////////////////////////////
        //
        // initial setup of the sim900
        //

        case CMD_FIRST_PWRON:
            switch (sim900.next_state) {
                case SIM900_IDLE:
                    sim900.next_state = SIM900_AT;
                    sim900.rc = RC_NULL;
                    sm_c = 0;
                    timer_a0_delay_noblk_ccr2(_1s); // ~1s
                break;
                case SIM900_AT:
                    if (sim900.rc == RC_OK) {
                        timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                        sim900_tx_cmd("AT+IPR=9600;+IFC=2,2;E0&W\r", 26, REPLY_TMOUT);
                        sim900.next_state = SIM900_WAITREPLY;
                    } else {
                        sm_c++;
                        timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                        sim900_tx_cmd("AT\r", 3, REPLY_TMOUT);
                    }
                    if (sm_c > 15) {
                        // something terribly wrong, stop sim900
                        sim900.cmd = CMD_OFF;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_WAITREPLY:
                    if (sim900.rc == RC_OK) {
                        sim900.cmd = CMD_OFF;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                        sim900.rdy |= NEED_SYSTEM_REBOOT;
                        //sim900.cmd = CMD_NULL;
                        //sim900.next_state = SIM900_IDLE;
                    } else {
                        sim900.cmd = CMD_OFF;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
            }
        break;

        ///////////////////////////////////
        //
        // poweroff of the sim900
        //

        case CMD_OFF:
            switch (sim900.next_state) {
                default:
                    sim900.next_state = SIM900_VBAT_OFF;
                    timer_a0_delay_noblk_ccr2(_5sp);
                    sim900_tx_cmd("AT+CPOWD=1\r", 11, _5s);
                break;
                case SIM900_VBAT_OFF:
                    sim900.next_state = SIM900_OFF;
                    timer_a0_delay_noblk_ccr2(_3s);
                    P1DIR &= 0xb7; // make RTS, DTR inputs
                    P4SEL &= 0xcf; // make both gprs RX and TX inputs
                    P4DIR &= 0xcf;
                    SIM900_VBAT_DISABLE;
                    SIM900_PWRKEY_HIGH;
                break;
                case SIM900_OFF:
                    sim900.checks = 0;
                    sim900.cmd = CMD_NULL;
                    if (sim900.rdy & NEED_SYSTEM_REBOOT) {
                        // use the wrong watchdog password, thus trigger a 
                        // full system reset
                        WDTCTL = WDTHOLD; 
                    }
                    sim900.rdy = 0;
                    sim900.flags &= ~TASK_IN_PROGRESS;
                break;
            }
        break;

        ///////////////////////////////////
        //
        // start GPRS session
        //
        
        case CMD_START_GPRS:
            switch (sim900.next_state) {
                case SIM900_IP_INITIAL:
                    sim900.next_state = SIM900_IP_START;
                    timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                    sim900_tx_str("AT+CGDCONT=1,\"IP\",\"", 19);
                    sim900_tx_str(s.apn, s.apn_len);
                    sim900_tx_cmd("\";+CIPSTATUS\r", 13, REPLY_TMOUT);
                break;
                case SIM900_IP_START:
                    if (sim900.rc == RC_STATE_IP_INITIAL) {
                        sim900.next_state = SIM900_IP_GPRSACT;
                        timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                        sim900_tx_str("AT+CSTT=\"", 9);
                        sim900_tx_str(s.apn, s.apn_len);
                        sim900_tx_str("\",\"", 3);
                        sim900_tx_str(s.user, s.user_len);
                        sim900_tx_str("\",\"", 3);
                        sim900_tx_str(s.pass, s.pass_len);
                        sim900_tx_cmd("\";+CIPSTATUS\r", 13, REPLY_TMOUT);
                    } else {
                        sim900.cmd = CMD_CLOSE_GPRS;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_IP_GPRSACT:
                    if (sim900.rc == RC_STATE_IP_START) {
                        sim900.next_state = SIM900_IP_STATUS;
                        timer_a0_delay_noblk_ccr2(_3sp);
                        sim900_tx_cmd("AT+CIICR;+CIPSTATUS\r", 20, _3s);
                    } else {
                        sim900.err |= ERR_GPRS_NO_IP_START;
                        sim900.cmd = CMD_CLOSE_GPRS;
                        // dont attempt any more connections to towers for a period
                        // since we probably got blacklisted
                        sim900.flags |= BLACKOUT;
                        gprs_blackout_lift = rtca_time.sys + BLACKOUT_GPRS_NO_IP_START;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_IP_STATUS:
                    if (sim900.rc == RC_STATE_IP_GPRSACT) {
                        sim900.next_state = SIM900_IP_CONNECT;
                        timer_a0_delay_noblk_ccr2(_3sp);
                        sim900_tx_cmd("AT+CIFSR;+CIPHEAD=1;+CIPSTATUS\r", 31, _3s);
                    } else {
                        sim900.cmd = CMD_CLOSE_GPRS;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_IP_CONNECT:
                    if (sim900.rc == RC_STATE_IP_STATUS) {
                        sim900.task_rv = SUBTASK_START_GPRS_OK;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY); // signal high level sm
                    } else {
                        sim900.cmd = CMD_CLOSE_GPRS;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;

            }
        break;

        ///////////////////////////////////
        //
        // establish TCP connection to the tracking server
        // and transmit HTTP POST packets
        //
        
        case CMD_POST_GPRS:
            switch (sim900.next_state) {
                case SIM900_TCP_START:
                    sim900.next_state = SIM900_IP_CONNECT_OK;
                    timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                    sim900_tx_str("AT+CIPSTART=\"TCP\",\"", 19);
                    sim900_tx_str(s.server, s.server_len);
                    snprintf(str_temp, STR_LEN, "\",\"%u\"\r", s.port);
                    sim900_tx_cmd(str_temp, strlen(str_temp), REPLY_TMOUT);
                break;
                case SIM900_IP_CONNECT_OK:
                    if (sim900.rc == RC_OK) {
                        // CIPSTART sends a quick OK and then a CONNECT OK a couple seconds later
                        sim900.next_state = SIM900_IP_SEND;
                        
                        sim900.cmd_type = CMD_SOLICITED;
                        sim900.rc = RC_NULL;
                        sim900.console = TTY_RX_WAIT;
                        timer_a0_delay_noblk_ccr3(_3s);
                        timer_a0_delay_noblk_ccr2(_3sp);
                    } else {
                        sim900.next_state = SIM900_TCP_CLOSE;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_IP_SEND:
                    if (sim900.rc == RC_STATE_IP_CONNECT) {
                        sim900.next_state = SIM900_IP_PUT;
                        timer_a0_delay_noblk_ccr2(SM_R_DELAY);

                        // payload contains everything after the HTTP header
                        //  - version (2 bytes), imei (15 bytes), settings (2 bytes), etc
                        //payload_size = fm24_data_len(m.ntx, m.e);
                        payload_size = fm24_data_len(m.seg[0], m.seg[1]);

                        sim900_tx_str("AT+CIPSEND=", 11);
                        // HTTP header is 19 + 6 + s.server_len + 2 + 34 + 25 = 86 bytes + s.server_len
                        snprintf(str_temp, STR_LEN, "%lu\r", payload_size + 86 + s.server_len);
                        sim900_tx_cmd(str_temp, strlen(str_temp), REPLY_TMOUT);
                    } else {
                        sim900.next_state = SIM900_TCP_CLOSE;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_IP_PUT:
                    if (sim900.rc == RC_TEXT_INPUT) {
                        sim900.next_state = SIM900_SEND_OK;
                        timer_a0_delay_noblk_ccr2(_10sp);
                        sim900_tx_str("POST /u1 HTTP/1.0\r\n", 19);
                        sim900_tx_str("Host: ", 6);
                        sim900_tx_str(s.server, s.server_len);
                        sim900_tx_str("\r\n", 2);
                        sim900_tx_str("Content-Type: application/binary\r\n", 34);
                        snprintf(str_temp, STR_LEN, "Content-Length: %05lu\r\n\r\n", payload_size);
                        sim900_tx_str(str_temp, strlen(str_temp));

                        for (i=0; i<payload_size; i++) {
                            fm24_read_from((uint8_t *)str_temp, m.seg[0] + i, 1);
                            if (i != payload_size - 1) {
                                sim900_tx_str(str_temp, 1);
                            } else {
                                sim900_tx_cmd(str_temp, 1, _10s);
                            }
                        }
                    } else {
                        sim900.next_state = SIM900_TCP_CLOSE;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_SEND_OK:
                    if (sim900.rc == RC_SEND_OK) {
                        sim900.next_state = SIM900_HTTP_REPLY;
                        
                        sim900.cmd_type = CMD_SOLICITED;
                        sim900.rc = RC_NULL;
                        sim900.console = TTY_RX_WAIT;
                        timer_a0_delay_noblk_ccr3(_3s);
                        timer_a0_delay_noblk_ccr2(_3sp);
                    } else {
                        sim900.next_state = SIM900_TCP_CLOSE;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_HTTP_REPLY:
                    // instead of waiting for '200 OK' maybe focus on 
                    // the 'RCVD OK' string sent from the application?
                    //if ((sim900.rc == RC_200_OK) || (s.settings & CONF_IGNORE_SRV_REPLY)) {
                    if ((sim900.rc == RC_RCVD_OK) || (s.settings & CONF_IGNORE_SRV_REPLY)) {
                        sim900.task_rv = SUBTASK_HTTP_POST_OK;
                        sim900.err = 0; // XXX move to store_pkt()

                        // remove the segment that has just been transmitted
                        for (i = 0; i < MAX_SEG; i++) {
                            m.seg[i] = m.seg[i+1];
                        }

                        if (m.seg_num > 1) {
                            m.seg_num--;
                        }
#ifdef DEBUG_GPRS
                        snprintf(str_temp, STR_LEN, "seg[0]: %lu\tseg[1]: %lu\tseg_num: %d\r\n", m.seg[0], m.seg[1], m.seg_num);
                        uart0_tx_str(str_temp, strlen(str_temp));
#endif
                    }
                    sim900.next_state = SIM900_TCP_CLOSE;
                    timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                break;
                case SIM900_TCP_CLOSE:
                    sim900.next_state = SIM900_CLOSE_CMD;
                    timer_a0_delay_noblk_ccr2(_3sp);
                    sim900_tx_cmd("AT+CIPCLOSE\r", 12, _3s);
                break;
                case SIM900_CLOSE_CMD:
                    sim900.next_state = SIM900_IDLE;
                    timer_a0_delay_noblk_ccr1(SM_STEP_DELAY); // signal high level sm
                break;
            }
        break;

        ///////////////////////////////////
        //
        // end GPRS session
        //
        
        case CMD_CLOSE_GPRS:
            switch (sim900.next_state) {
                default:
                    sim900.next_state = SIM900_CLOSE_CMD;
                    timer_a0_delay_noblk_ccr2(_3sp);
                    sim900_tx_cmd("AT+CIPSHUT\r", 11, _3s);
                break;
                case SIM900_CLOSE_CMD:
                    sim900.next_state = SIM900_IDLE;
                    sim900.task_rv = SUBTASK_CLOSE_GPRS_OK;
                    timer_a0_delay_noblk_ccr1(SM_STEP_DELAY); // signal high level sm
                break;
            }
        break;

        ///////////////////////////////////
        //
        // send SMS
        //

        case CMD_SEND_SMS:
            switch (sim900.next_state) {
                default:
                    sim900.next_state = SIM900_SET1;
                    timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                    sim900_tx_cmd("AT+CMGF=1\r", 10, REPLY_TMOUT);
                break;
                case SIM900_SET1:
                    if (sim900.rc == RC_OK) {
                        sim900.next_state = SIM900_TEXT_INPUT;
                        timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                        sim900_tx_str("AT+CMGS=\"", 9);
                        sim900_tx_str(s.ctrl_phone, s.ctrl_phone_len);
                        sim900_tx_cmd("\"\r", 2, REPLY_TMOUT);
                    }
                break;
                case SIM900_TEXT_INPUT:
                    if (sim900.rc == RC_TEXT_INPUT) {
                        sim900.next_state = SIM900_CLOSE_CMD;
                        timer_a0_delay_noblk_ccr2(_6s);
                        switch (sim900.sms_queue[sim900.current_s]) {
                            case SMS_GENERIC_SETUP:
                                snprintf(str_temp, STR_LEN, "setup 0x%04x\r", s.settings);
                                sim900_tx_str(str_temp, strlen(str_temp));
                            break;
                            case SMS_GPRS_SETUP:
                                sim900_tx_str("aup=\"", 5);
                                sim900_tx_str(s.apn, s.apn_len);
                                sim900_tx_str("\",\"", 3);
                                sim900_tx_str(s.user, s.user_len);
                                sim900_tx_str("\",\"", 3);
                                sim900_tx_str(s.pass, s.pass_len);
                                sim900_tx_str("\" srv=\"", 7);
                                sim900_tx_str(s.server, s.server_len);
                                snprintf(str_temp, STR_LEN, ":%u\"\r", s.port);
                                sim900_tx_str(str_temp, strlen(str_temp));
                            break;
                            case SMS_GPRS_TIMINGS:
                                snprintf(str_temp, STR_LEN, "sml %u smst %u smmt %u\r", s.gprs_loop_interval, s.gprs_static_tx_interval, s.gprs_moving_tx_interval);
                                sim900_tx_str(str_temp, strlen(str_temp));
                            break;
                            case SMS_GPS_TIMINGS:
                                snprintf(str_temp, STR_LEN, "spl %u spw %u spi %u spg %u\r", s.gps_loop_interval, s.gps_warmup_interval, s.gps_invalidate_interval, s.geofence_trigger);
                                sim900_tx_str(str_temp, strlen(str_temp));
                            break;
                            case SMS_DEFAULTS:
                                settings_init(SEGMENT_B, FACTORY_DEFAULTS);
                                sim900.rdy |= NEED_SYSTEM_REBOOT;
                                flash_save(SEGMENT_B, (void *)&s, sizeof(s));
                                sim900_tx_str("defaults loaded\r", 16);
                            break;
                            case SMS_ERRORS:
                                if (sim900.err) {
                                    snprintf(str_temp, STR_LEN, "err 0x%04x\r", sim900.err);
                                    sim900_tx_str(str_temp, strlen(str_temp));
                                    sim900.err = 0;
                                } else {
                                    sim900_tx_str("no errors\r", 10);
                                }
                            break;
                            case SMS_CODE_OK:
                                sim900_tx_str("code ok\r", 8);
                            break;
                            case SMS_VREF:
                                snprintf(str_temp, STR_LEN, "vref %d vbat %d.%02dV vraw %d.%02dV\r",
                                    s.vref, stat.v_bat/100, stat.v_bat%100, 
                                    stat.v_raw/100, stat.v_raw%100);
                                sim900_tx_str(str_temp, strlen(str_temp));
                            break;
                        }
                        sim900_tx_cmd(eom, 2, _5s);
                    }
                break;
                case SIM900_CLOSE_CMD:
                    if (sim900.rc == RC_CMGS) {
                        sim900.current_s++;
                        sim900.next_state = SIM900_IDLE;
                        sim900.cmd = CMD_NULL;
                        sim900.task_rv = SUBTASK_SEND_SMS_OK;
                        timer_a0_delay_noblk_ccr1(SM_R_DELAY); // signal high level sm
                    }
                break;
            }
        break;

        ///////////////////////////////////
        //
        // check if we got unread SMSs and read the first one
        //

        case CMD_PARSE_SMS:
            switch (sim900.next_state) {
                case SIM900_IDLE:
                    sim900.next_state = SIM900_SET1;
                    timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                    sim900_tx_cmd("AT+CMGF=1\r", 10, REPLY_TMOUT);
                break;
                case SIM900_SET1:
                    sim900.next_state = SIM900_PARSE_SMS;
                    timer_a0_delay_noblk_ccr2(_5sp);
                    sim900_tx_cmd("AT+CMGL=\"ALL\",\r", 15, _5s);
                break;
                case SIM900_PARSE_SMS:
                    if (sim900.rc == RC_CMGL) {
                        timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                        sim900_tx_str("AT+CMGR=", 8);
                        sim900_tx_str(sim900.rcvd_sms_id, sim900.rcvd_sms_id_len);
                        sim900_tx_cmd(",1\r", 3, REPLY_TMOUT);
                        sim900.next_state = SIM900_DEL_SMS;
                        // deincrement task queue so we parse SMSs again
                        sim900.current_t--;
                    } else {
                        // no more SMSs to parse, exit
                        sim900.next_state = SIM900_CLOSE_CMD;
                        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
                    }
                break;
                case SIM900_DEL_SMS:
                    if (sim900.rc == RC_CMGR) {
                        timer_a0_delay_noblk_ccr2(_5sp);
                        sim900_tx_str("AT+CMGD=", 8);
                        sim900_tx_str(sim900.rcvd_sms_id, sim900.rcvd_sms_id_len);
                        sim900_tx_cmd(",0\r", 3, _5s);
                        sim900.next_state = SIM900_CLOSE_CMD;
                    }
                break;
                case SIM900_CLOSE_CMD:
                    sim900.task_rv = SUBTASK_PARSE_SMS_OK;
                    sim900.next_state = SIM900_IDLE;
                    sim900.cmd = CMD_NULL;
                    timer_a0_delay_noblk_ccr1(SM_STEP_DELAY); // signal high level sm
                break;
            }
        break;

        ///////////////////////////////////
        //
        // check if we got unread SMSs and read the first one
        //

        case CMD_PARSE_CENG:
            switch (sim900.next_state) {
                case SIM900_IDLE:
                    sim900.next_state = SIM900_SET_CENG;
                    timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                    sim900_tx_cmd("AT+CENG=2,1\r", 12, REPLY_TMOUT);
                break;
                case SIM900_SET_CENG:
                    if (sim900.rc == RC_OK) {
                        // CENG=2,1 sends a quick OK and then a long +CENG list seconds later
                        sim900.next_state = SIM900_WAITREPLY;
                        sim900.cmd_type = CMD_SOLICITED;
                        sim900.rc = RC_NULL;
                        sim900.console = TTY_RX_WAIT;
                        // wait ~10s for the unsolicited '+CENG:.*' messages
                        timer_a0_delay_noblk_ccr3(_10s);
                        timer_a0_delay_noblk_ccr2(_10sp);
                    }
                break;
                case SIM900_WAITREPLY:
                    if (sim900.rc == RC_CENG_RCVD) {
                        sim900.next_state = SIM900_CLOSE_CMD;
                        timer_a0_delay_noblk_ccr2(SM_R_DELAY);
                        sim900_tx_cmd("AT+CENG=0,1\r", 12, REPLY_TMOUT);
                    }
                break;
                case SIM900_CLOSE_CMD:
                    if (sim900.rc == RC_OK) {
                        sim900.task_rv = SUBTASK_PARSE_CENG_OK;
                        sim900.next_state = SIM900_IDLE;
                        sim900.cmd = CMD_NULL;
                        timer_a0_delay_noblk_ccr1(SM_STEP_DELAY); // signal high level sm
                    }
                break;
            }
        break;

        default:
        break;
    }
}

static void sim900_console_timing(enum sys_message msg)
{
    if (sim900.console == TTY_RX_WAIT) {
        // this point is reached REPLY_TMOUT ticks after a command was sent
        // it also means SIM900 failed to reply in that time interval
        sim900.rc = RC_TMOUT;
        sim900.console = TTY_NULL;
        SIM900_RTS_LOW;
    } else if (sim900.console == TTY_RX_PENDING) {
        // this point is reached if there is INTRCHAR_TMOUT ticks worth of silence
        // after the last byte that was received
        uart1_rx_enable = false;
        sim900.console = TTY_NULL;
        uart1_last_event |= UART1_EV_RX;
    }
}

uint16_t sim900_tx_str(char *str, const uint16_t size)
{
    uint16_t p = 0;
    
#ifdef DEBUG_GPRS
    uart0_tx_str(str, size);
#endif

    while (p < size) {
        while (!(SIM900_UCAIFG & UCTXIFG)) {
            if (timer_a0_last_event & 0xe) {
                return p;
            }
        }
        if (timer_a0_last_event & 0xe) {
            return p;
        }
        // TX buffer ready?
        if (!(SIM900_CTS_IN)) {
            SIM900_UCATXBUF = str[p];
            p++;
        }
    }
    return p;
}

uint16_t sim900_tx_cmd(char *str, const uint16_t size, const uint16_t reply_tmout)
{
    uint16_t p = 0;

    if (sim900.console != TTY_NULL) {
        return EXIT_FAILURE;
    }

#ifdef DEBUG_GPRS
    uart0_tx_str(str, size);
#endif

    sim900.cmd_type = CMD_SOLICITED;
    sim900.rc = RC_NULL;
    sim900.console = TTY_RX_WAIT;
    // set up timer that will end the wait for a reply
    timer_a0_delay_noblk_ccr3(reply_tmout);

    while (p < size) {
        while (!(SIM900_UCAIFG & UCTXIFG)) {
            if (timer_a0_last_event & 0xe) {
                return p;
            }
        }
        if (timer_a0_last_event & 0xe) {
            return p;
        }
        // TX buffer ready? and no timers triggered?
        if (!(SIM900_CTS_IN)) { // hw flow control allows TX?
            SIM900_UCATXBUF = str[p];
            p++;
        }
    }

    return p;
}

uint8_t sim900_parse_rx(char *str, const uint16_t size)
{
    uint8_t i;

    str[size] = 0;
    char * found;

    if (sim900.cmd_type == CMD_SOLICITED) {
        if (strstr(str, "+CMGS:")) {
            // '\r\n+CMGS: 5\r\n\r\nOK\r\n'
            // we want to catch the +CMGS part, so parse before 'else if == "OK"'
            sim900.rc = RC_CMGS;
        } else if (strstr(str, "+CMGL:")) {
            // '\r\n+CMGL: 19,"REC READ","+40 ...'
            found = strstr(str, "+CMGL:");
            i = 0;
            while ((found[i+7] != ',') && (i<3)) {
                sim900.rcvd_sms_id[i] = found[i+7];
                i++;
            }
            sim900.rcvd_sms_id_len = i;
            sim900.rc = RC_CMGL;
            sim900.cmd_type = CMD_IGNORE;
        } else if (strstr(str, "+CMGR:")) {
            sim900_parse_sms(str, size);
            sim900.rc = RC_CMGR;
            // just in case the reply overflows
            //sim900.cmd_type = CMD_IGNORE;
        } else if (strstr(str, "+CENG:0")) {
            sim900_parse_ceng(str, size);
            sim900.rc = RC_CENG_RCVD;
            // in case the reply overflows
            sim900.cmd_type = CMD_IGNORE;
        } else if (strstr(str, "IP INITIAL")) {
            // '\r\nOK\r\n\r\nSTATE: IP INITIAL\r\n'
            sim900.rc = RC_STATE_IP_INITIAL;
        } else if (strstr(str, "IP START")) {
            // '\r\nOK\r\n\r\nSTATE: IP START\r\n'
            sim900.rc = RC_STATE_IP_START;
        } else if (strstr(str, "IP GPRSACT")) {
            // '\r\nOK\r\n\r\nOK\r\n\r\nSTATE: IP GPRSACT\r\n
            sim900.rc = RC_STATE_IP_GPRSACT;
        } else if (strstr(str, "IP STATUS")) {
            // '\r\nAAA.BBB.CCC.DDD\r\n\r\nOK\r\nSTATE: IP STATUS\r\n
            sim900.rc = RC_STATE_IP_STATUS;
        } else if (strstr(str, "CONNECT OK")) {
            // CONNECT OK
            sim900.rc = RC_STATE_IP_CONNECT;
        } else if (strstr(str, "SEND OK")) {
            // '\r\nSEND OK\r\n'
            sim900.rc = RC_SEND_OK;
        } else if (strstr(str, "RCVD OK")) {
            // 'HTTP/1.1 200 OK\r\nConnection: close\r\n\r\nRCVD OK\r\n'
            sim900.rc = RC_RCVD_OK;
        } else if (strstr(str, "200 OK")) {
            // '\r\n+IPD,141:HTTP/1.1 200 OK\r\n'
            sim900.rc = RC_200_OK;
        } else if (strstr(str, "SHUT")) {
            // SHUT OK
            sim900.rc = RC_STATE_IP_SHUT;
        } else if (strstr(str, "OK")) {
            // '\r\nOK\r\n'
            sim900.rc = RC_OK;
        } else if (strstr(str, "ERROR")) {
            sim900.rc = RC_ERROR;
        } else if (strstr(str, "> ")) {
            // '\r\n> '
            sim900.rc = RC_TEXT_INPUT;
        } else {
            // unknown solicited? reply
            sim900.rc = RC_NULL;
        }

        if (sim900.cmd_type != CMD_IGNORE) {
            // shorten the state machine delay
            timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
        }

    } else if ((sim900.cmd_type == CMD_SOLICITED_GSN) && (size == 25)) {
        for (i=2; i<17; i++) {
            sim900.imei[i-2] = str[i];
        }
        sim900.rc = RC_IMEI_RCVD;
        timer_a0_delay_noblk_ccr2(SM_STEP_DELAY); // signal low level sm
    } else if (sim900.cmd_type == CMD_UNSOLICITED) {
        if (strstr(str, "RDY")) {
            // '\r\nRDY\r\n'
            sim900.rdy |= RDY;
        } else if (strstr(str, "+CPIN:")) {
            if (strstr(str, "READY")) {
                sim900.rdy |= PIN_RDY;
            }
        } else if (strstr(str, "Call Ready")) {
            // '\r\nCall Ready\r\n'
            sim900.rdy |= CALL_RDY;
            // we are listening for this, so make sure the state machine 
            // is pinged quicker
            timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
        }
        sim900.rc = RC_NULL;
    }

    // signal that we are ready to receive more
    SIM900_RTS_LOW;

    uart1_p = 0;
    uart1_rx_enable = true;  

    if (sim900.cmd_type != CMD_IGNORE) {
        sim900.cmd_type = CMD_UNSOLICITED;
    }
    return EXIT_SUCCESS;
}

uint8_t sim900_parse_ceng(char *str, const uint16_t size)
{
    uint8_t i, rv;
    char *seek;
    uint16_t num=0;

    //+CENG:0,"0049,40,99,226,01,32,882f,00,05,2b7e,255"
    //+CENG:1,"0037,38,32,882e,226,01,2b7e"
    //+CENG:2,"0073,24,15,b2ff,226,01,2b7e"
    //+CENG:3,"0063,22,15,b2fe,226,01,2b7e"
    
    // the currently used cell (cell 0) has 11 elements in it's data structure

    seek = strstr(str, "+CENG:0");
    seek += 9;

    // ignore arcfn
    rv = extract_dec(seek, &num);
    seek += rv + 1;
    // rxl
    rv = extract_dec(seek, &sim900.cell[0].rxl);
    seek += rv + 1;
    // ignore rxq
    rv = extract_dec(seek, &num);
    seek += rv + 1;
    // mcc
    rv = extract_dec(seek, &sim900.cell[0].mcc);
    seek += rv + 1;
    // mnc
    rv = extract_dec(seek, &sim900.cell[0].mnc);
    seek += rv + 1;
    // ignore bsic
    rv = extract_dec(seek, &num);
    seek += rv + 1;
    // cellid
    rv = extract_hex(seek, &sim900.cell[0].cellid);
    seek += rv + 1;
    // ignore rla
    rv = extract_dec(seek, &num);
    seek += rv + 1;
    // ignore txp
    rv = extract_dec(seek, &num);
    seek += rv + 1;
    // lac
    rv = extract_hex(seek, &sim900.cell[0].lac);

    seek = strstr(str, "+CENG:1");

    for (i=1;i<4;i++) {
        seek += 9;

        // ignore arcfn
        rv = extract_dec(seek, &num);
        seek += rv + 1; // ,
        // rxl
        rv = extract_dec(seek, &sim900.cell[i].rxl);
        seek += rv + 1; // ,
        // ignore bsic
        rv = extract_dec(seek, &num);
        seek += rv + 1; // ,
        // cellid
        rv = extract_hex(seek, &sim900.cell[i].cellid);
        seek += rv + 1; // ,
        // mcc
        rv = extract_dec(seek, &sim900.cell[i].mcc);
        seek += rv + 1; // ,
        // mnc
        rv = extract_dec(seek, &sim900.cell[i].mnc);
        seek += rv + 1; // ,
        // lac
        rv = extract_hex(seek, &sim900.cell[i].lac);
        seek += rv + 3; // "\r\n
    }
 
    return EXIT_SUCCESS;
}

uint8_t sim900_parse_sms(char *str, const uint16_t size)
{
    uint8_t i=0,seek;
    char sender[MAX_PHONE_LEN];
    uint8_t sender_len;
    uint8_t save = false;
    uint16_t tmp;
    char code[4];
    char *p;

    // find out who is calling
    // +CMGL: 1,"REC READ","+40555000001","","14/07/30,10:52:07+12"
    //                       ^ sender

    for ( seek=0; seek<size; seek++ ) {
        if (str[seek] == '"') {
            i++;
        }
        if (i == 3) {
            seek++;
            break;
        }
    }

    i=0;
    while ((str[seek+i] != '"') && (i<MAX_PHONE_LEN-1)) {
        sender[i] = str[seek+i];
        i++;
    }
    sender[i] = 0;
    sender_len = i;

    if (strstr(str, "code")) {
        extract_str(str, "code", code, &i, 4);
        // if the code is the last 4 digits from the imei, then authorize 
        // the sender's number
        if ((sim900.imei[11] == code[0]) && (sim900.imei[12] == code[1]) && 
              (sim900.imei[13] == code[2]) && (sim900.imei[14] == code[3]))
        {
            for ( i=0; i<sender_len; i++ ) {
                s.ctrl_phone[i] = sender[i];
            }
            s.ctrl_phone_len = sender_len;
            s.ctrl_phone[s.ctrl_phone_len] = 0; // needed for strstr
            save = true;
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_CODE_OK);
        }
    }

    if (s.ctrl_phone[0] == 0) {
        return EXIT_SUCCESS;
    }

    if (strstr(sender, s.ctrl_phone)) {
        // the authorized phone sent us a command
        // XXX should make sure add_subtask does not end in failure!
        if (strstr(str, "err?")) {
            // send sim900.err in a sms reply
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_ERRORS);
        } else if (strstr(str, "gprs?")) {
            // send the gprs credentials in a sms reply
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPRS_SETUP);
        } else if (strstr(str, "setup?")) {
            // send the generic setup in a sms reply
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GENERIC_SETUP);
        } else if (strstr(str, "spt?")) {
            // send the gps related timings in a sms reply
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPS_TIMINGS);
        } else if (strstr(str, "smt?")) {
            // send the gprs related timings in a sms reply
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPRS_TIMINGS);
        } else if (strstr(str, "apn")) {
            extract_str(str, "apn", s.apn, &s.apn_len, MAX_APN_LEN);
            save = true;
        } else if (strstr(str, "user")) {
            extract_str(str, "user", s.user, &s.user_len, MAX_USER_LEN);
            save = true;
        } else if (strstr(str, "pass")) {
            extract_str(str, "pass", s.pass, &s.pass_len, MAX_PASS_LEN);
            save = true;
        } else if (strstr(str, "srv")) {
            extract_str(str, "srv", s.server, &s.server_len, MAX_SERVER_LEN);
            save = true;
        } else if (strstr(str, "port")) {
            p = strstr(str, "port");
            p += 4;
            extract_dec(p, &s.port);
            save = true;
        } else if (strstr(str, "spl")) {
            p = strstr(str, "spl");
            p += 3;
            extract_dec(p, &s.gps_loop_interval);
            save = true;
            gps_trigger_next = 0;
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPS_TIMINGS);
        } else if (strstr(str, "spw")) {
            p = strstr(str, "spw");
            p += 3;
            extract_dec(p, &s.gps_warmup_interval);
            save = true;
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPS_TIMINGS);
        } else if (strstr(str, "spi")) {
            p = strstr(str, "spi");
            p += 3;
            extract_dec(p, &s.gps_invalidate_interval);
            save = true;
            if (s.gps_invalidate_interval > s.gps_loop_interval) {
                s.gps_invalidate_interval = s.gps_loop_interval;
            }
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPS_TIMINGS);
        } else if (strstr(str, "spg")) {
            p = strstr(str, "spg");
            p += 3;
            extract_dec(p, &s.geofence_trigger);
            save = true;
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPS_TIMINGS);
        } else if (strstr(str, "sml")) {
            p = strstr(str, "sml");
            p += 3;
            extract_dec(p, &s.gprs_loop_interval);
            save = true;
            gprs_trigger_next = rtca_time.sys + s.gprs_loop_interval;
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPRS_TIMINGS);
        } else if (strstr(str, "smst")) {
            p = strstr(str, "smst");
            p += 4;
            extract_dec(p, &s.gprs_static_tx_interval);
            save = true;
            gprs_tx_next = rtca_time.sys + s.gprs_static_tx_interval;
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPRS_TIMINGS);
        } else if (strstr(str, "smmt")) {
            p = strstr(str, "smmt");
            p += 4;
            extract_dec(p, &s.gprs_static_tx_interval);
            save = true;
            gprs_tx_next = rtca_time.sys + s.gprs_moving_tx_interval;
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_GPRS_TIMINGS);
        } else if (strstr(str, "set")) {
            p = strstr(str, "set");
            p += 3;
            extract_dec(p, &s.settings);
            save = true;
            settings_apply();
        } else if (strstr(str, "ping")) {
            sim900.flags |= TX_FIX_RDY;
            if (fm24_data_len(m.seg[0], m.seg[1]) > 0) {
                sim900_add_subtask(SUBTASK_TX_GPRS, SMS_NULL);
            }
        } else if (strstr(str, "default")) {
            sim900_add_subtask(SUBTASK_SEND_SMS, SMS_DEFAULTS);
        } else if (strstr(str, "vref")) {
            p = strstr(str, "vref");
            p += 4;
            extract_dec(p, &tmp);
            if ((tmp < 210) && (tmp > 190)) {
                s.vref = tmp;
                save = true;
                adc_read();
                // send the received value back in a sms reply
                sim900_add_subtask(SUBTASK_SEND_SMS, SMS_VREF);
            }
        }


    }

    if (save) {
        flash_save(SEGMENT_B, (void *)&s, sizeof(s));
    }

    return EXIT_SUCCESS;
}

void sim900_init_messagebus(void)
{
    sys_messagebus_register(&sim900_tasks, SYS_MSG_TIMER0_CRR1);
    sys_messagebus_register(&sim900_state_machine, SYS_MSG_TIMER0_CRR2);
    sys_messagebus_register(&sim900_console_timing, SYS_MSG_TIMER0_CRR3);
}

void sim900_start(void)
{
    sim900.cmd = CMD_ON;
    sim900.next_state = SIM900_VBAT_ON;
    timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
}

void sim900_halt(void)
{
    sim900.cmd = CMD_OFF;
    timer_a0_delay_noblk_ccr2(SM_STEP_DELAY);
}

uint8_t sim900_add_subtask(sim900_task_state_t subtask, sim900_sms_subj_t sms_subj)
{

    if (subtask == SUBTASK_SEND_SMS) {
        if (sim900.last_sms < SMS_QUEUE_SIZE - 1) {
            sim900.sms_queue[sim900.last_sms] = sms_subj;
            sim900.last_sms++;
        } else {
            sim900.err |= ERR_TASK_ADD;
            return EXIT_FAILURE;
        }
    }

    if (sim900.last_t < TASK_QUEUE_SIZE - 1) {
        sim900.queue[sim900.last_t] = subtask;
        sim900.last_t++; 
    } else {
        sim900.err |= ERR_TASK_ADD;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;

}

void sim900_exec_default_task(void)
{
    sim900.task = TASK_DEFAULT;
    sim900.task_next_state = SUBTASK_ON;
    sim900.current_t = 0;
    sim900.last_t = 0;
    sim900.current_s = 0;
    sim900.last_sms = 0;
    if (!sim900.imei[0]) {
        sim900_add_subtask(SUBTASK_GET_IMEI, SMS_NULL);
    }
    sim900_add_subtask(SUBTASK_PARSE_SMS, SMS_NULL);
    sim900_add_subtask(SUBTASK_PARSE_CENG, SMS_NULL);
    if (sim900.flags & TX_FIX_RDY) {
        if (fm24_data_len(m.seg[0], m.seg[1]) > 0) {
            sim900_add_subtask(SUBTASK_TX_GPRS, SMS_NULL);
        } else {
            sim900.flags &= ~TX_FIX_RDY;
        }
    }
    timer_a0_delay_noblk_ccr1(SM_STEP_DELAY);
}

//////////////////////////////////////////////////////////
//
// helper functions
//

uint8_t extract_dec(char *str, uint16_t *rv)
{
    uint8_t i=0;
    char *p = str;
    char c = *p;
    
    *rv = 0;

    // ignore spaces before the number
    while (c == 32) {
        c = *++p;
    }

    while ((i<6) && (c > 47) && (c < 58)) {
        *rv *= 10;
        *rv += c - 48;
        i++;
        //p++;
        c = *++p;
    }

    return i;
}

uint8_t extract_hex(char *str, uint16_t *rv)
{
    uint8_t i=0;
    char *p = str;
    char c = *p;
    
    *rv = 0;

    // ignore spaces before the number
    while (c == 32) {
        c = *++p;
    }

    while ((i<4) && (((c > 47) && (c < 58)) || ((c > 96) && (c < 103)) || ((c > 64) && (c < 71)))) {

        // go lowercase (A-F -> a-f)
        if ((c > 64) && (c < 71)) {
            c += 32;
        }

        *rv = *rv << 4;
        if ((c > 47) && (c < 58)) {
            *rv += c - 48;
        } else if ((c > 96) && (c < 103)) {
            *rv += c - 87;
        }
        i++;
        //p++;
        c = *++p;
    }

    return i;
}


void extract_str(const char *haystack, const char *needle, char *str, uint8_t *len, const uint8_t max_len)
{
    char *seek;
    uint8_t i=0;

    seek = strstr(haystack, needle);
    seek += strlen(needle);

    if (seek[0] == 0x0d) {
        *len = 0;
    }

    while (seek[0] != 0x0d) {
        if (i == max_len) {
            break;
        }
        if (seek[0] == ' ') {
            // ignore spaces
        } else if (seek[0] == ',') {
            // my phone is unable to send full stops
            // but can send commas
            str[i] = '.';
            i++;
            *len = i;
        } else {
            str[i] = seek[0];
            i++;
            *len = i;
        }
        seek++;
    }
}


