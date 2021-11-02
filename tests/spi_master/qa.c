
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "hi_stirrer.h"
#include "spi_d.h"
#include "timer_a0.h"
#include "qa.h"

#define STR_LEN 64

char str_temp[STR_LEN+1];
uint8_t data[32];

void display_menu(void)
{
    uart0_print("\r\n SPI master test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m      - show menu\r\n");
    uart0_print(" \e[33;1minfo\e[0m   - get reg settings\r\n");
    uart0_print(" \e[33;1mr\e[0m      - read registers\r\n");
    uart0_print(" \e[33;1ms\e[0m      - set motor speed\r\n");
    uart0_print(" \e[33;1mg\e[0m      - get state\r\n");
    uart0_print(" \e[33;1mclr\e[0m    - clear err flags\r\n");
    uart0_print(" \e[33;1mfact\e[0m   - factory setup\r\n");
    uart0_print(" \e[33;1mrst\e[0m    - reset the uC\r\n");
    uart0_print(" \e[33;1mt1\e[0m     - t1 - read test\r\n");
    uart0_print(" \e[33;1mt2\e[0m     - t2 - write test\r\n");
    uart0_print(" \e[33;1mt3\e[0m     - t3 - read test\r\n");
}

uint8_t rpm_0[4] = {0x02, 0xfe, 0x00, 0x00};
uint8_t rpm_1000[4] = {0x02, 0xfe, 0x0a, 0xf6};
//uint8_t def_serial_no[12] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L' };
//uint8_t def_serial_no[12] = { 'H', '0', '0', '8', '9', '2', '9', '1', 0, 0, 0, 0 };
uint8_t def_serial_no[12] = { '8', '0', '0', '0', '5', '0', '0', '0', '4', '0', '1', '1' };


uint8_t read_t3(verbosity v_level)
{
    uint8_t checksum_err = 0;
    uint8_t reply[2];

    //timer_a0_delay_ccr2(2800);
    //timer_a0_delay_ccr2(2800);
    //timer_a0_delay_ccr2(2800);
    //timer_a0_delay_ccr2(2800);
 
    memset(&data, 0, 32);
    hist_read_reg(EUSCI_SPI_BASE_ADDR, HIST_rSTATUS, &reply[0], 2);
    if (! SPIVerifyCheckSum(reply, 2)) {
        snprintf(str_temp, STR_LEN, "ST %02x%02x\r\n", reply[0], reply[1]);
        uart0_print(str_temp);
        checksum_err = 1;
    }

    memset(&data, 0, 32);
    hist_read_reg(EUSCI_SPI_BASE_ADDR, HIST_rTARGET_RPM, &data[0], 3);
    if (! SPIVerifyCheckSum(data, 3)) {
        snprintf(str_temp, STR_LEN, "RX %02x%02x\r\n", data[0], data[1]);
        uart0_print(str_temp);
        checksum_err = 1;
    }

    return checksum_err;
}


uint8_t set_and_check_speed(verbosity v_level, const int16_t speed)
{
    uint8_t checksum_err = 0;
    uint8_t speed_err = 0;
    uint8_t blocked_err = 0;
    uint8_t reply[2];

    memset(&data, 0, 32);
    //data[1] = SPIComputeCheckSum(&(data[0]) , 1);
    data[0] = speed & 0xff; // LSB
    data[1] = speed >> 8;   // MSB
    data[2] = SPIComputeCheckSum(&(data[0]) , HIST_rTARGET_RPM_SZ);
    timer_a0_delay_ccr2(50);
    hist_write_reg(EUSCI_SPI_BASE_ADDR, HIST_rTARGET_RPM, &data[0], 3);

   
    memset(&data, 0, 32);

#if 0

    timer_a0_delay_ccr2(2800);
    timer_a0_delay_ccr2(2800);
    timer_a0_delay_ccr2(2800);
    timer_a0_delay_ccr2(1000);
 
    memset(&data, 0, 32);
    hist_read_reg(EUSCI_SPI_BASE_ADDR, HIST_rSTATUS, &reply[0], 2);
    if (! SPIVerifyCheckSum(reply, 2)) {
        //snprintf(str_temp, STR_LEN, "ST %02x%02x\r\n", reply[0], reply[1]);
        //uart0_print(str_temp);
        checksum_err = 1;
    }

    if (reply[0] & HIST_bmERR2) {
        snprintf(str_temp, STR_LEN, "ERR2 occured\r\n");
        uart0_print(str_temp);
        blocked_err = 1;
        memset(&data, 0, 32);
        data[0] = HIST_bmCLR;
        data[1] = SPIComputeCheckSum(data, 1);
        hist_write_reg(EUSCI_SPI_BASE_ADDR, HIST_rMODE, &data[0], 2);
        timer_a0_delay_ccr2(2800);
    }

    timer_a0_delay_ccr2(100);
    memset(&data, 0, 32);
    hist_read_reg(EUSCI_SPI_BASE_ADDR, HIST_rTARGET_RPM, &data[0], 3);
    if (! SPIVerifyCheckSum(data, 3)) {
        //snprintf(str_temp, STR_LEN, "RX %02x%02x\r\n", data[0], data[1]);
        //uart0_print(str_temp);
        checksum_err = 1;
    }
    if (((speed & 0xff) != data[0]) || ((speed >> 8) != data[1])) {
        speed_err = 1;
    }

    if (speed_err || checksum_err) {
        v_level = verbose;
    }

    if (v_level == verbose ) {
        snprintf(str_temp, STR_LEN, "TX %02x%02x%02x\r\n", speed & 0xff, speed >> 8, SPIComputeCheckSum((uint8_t *)&speed , 2));
        uart0_print(str_temp);
        snprintf(str_temp, STR_LEN, "RX %02x%02x%02x\r\n", data[0], data[1], data[2]);
        uart0_print(str_temp);
        if (speed_err) {
            uart0_print("   speed err\r\n");
        }
        if (checksum_err) {
            uart0_print("   checksum err\r\n");
        }
        if (blocked_err) {
            uart0_print("   blocked err\r\n");
        }
    }


#else
    timer_a0_delay_ccr2(2800);
    timer_a0_delay_ccr2(2800);
    timer_a0_delay_ccr2(2800);
    timer_a0_delay_ccr2(1000);
 

    hist_read_reg(EUSCI_SPI_BASE_ADDR, HIST_rSTATUS, &data[0], 7);
    if (! SPIVerifyCheckSum(data, 2)) {
        //snprintf(str_temp, STR_LEN, "ST %02x%02x\r\n", status[0], status[1]);
        //uart0_print(str_temp);
        checksum_err = 1;
    }

    if (data[0] & HIST_bmERR2) {
        snprintf(str_temp, STR_LEN, "ERR2 occured\r\n");
        uart0_print(str_temp);
        blocked_err = 1;
        memset(&reply, 0, 2);
        reply[0] = HIST_bmCLR;
        reply[1] = SPIComputeCheckSum(reply, 1);
        hist_write_reg(EUSCI_SPI_BASE_ADDR, HIST_rMODE, &reply[0], 2);
        timer_a0_delay_ccr2(2800);
    }

    if (! SPIVerifyCheckSum(&data[HIST_rTARGET_RPM], 3)) {
        //snprintf(str_temp, STR_LEN, "RX %02x%02x\r\n", data[0], data[1]);
        //uart0_print(str_temp);
        checksum_err = 1;
    }

    if (((speed & 0xff) != data[HIST_rTARGET_RPM]) || (((speed >> 8) & 0xff) != data[HIST_rTARGET_RPM+1])) {
        speed_err = 1;
    }

    if (speed_err || checksum_err) {
        v_level = verbose;
    }

    if (v_level == verbose ) {
        snprintf(str_temp, STR_LEN, "TX %02x%02x%02x\r\n", speed & 0xff, (speed >> 8) & 0xff, SPIComputeCheckSum((uint8_t *)&speed , 2));
        uart0_print(str_temp);
        snprintf(str_temp, STR_LEN, "RX %02x%02x%02x\r\n", data[HIST_rTARGET_RPM], data[HIST_rTARGET_RPM+1], data[HIST_rTARGET_CHK]);
        uart0_print(str_temp);
        if (speed_err) {
            uart0_print("   speed err\r\n");
        }
        if (checksum_err) {
            uart0_print("   checksum err\r\n");
        }
        if (blocked_err) {
            uart0_print("   blocked err\r\n");
        }
    }

#endif


    return checksum_err + speed_err + blocked_err;
}

uint8_t get_status(verbosity v_level)
{
    uint8_t i;
    uint8_t ret = EXIT_SUCCESS;

    memset(&data, 0, 32);
    timer_a0_delay_ccr2(50);
    hist_read_reg(EUSCI_SPI_BASE_ADDR, 0, &data[0], 10);
    if (! SPIVerifyCheckSum(data, HIST_rSTATUS_SZ + HIST_rCHECKSUM_SZ)) {
        ret++;
    }
    if (! SPIVerifyCheckSum(data+HIST_rSTATUS_SZ+HIST_rMODE_SZ + 2*HIST_rCHECKSUM_SZ, HIST_rTARGET_RPM_SZ + HIST_rCHECKSUM_SZ)) {
        ret++;
    }
    if (! SPIVerifyCheckSum(data+HIST_rSTATUS_SZ + HIST_rMODE_SZ + HIST_rTARGET_RPM_SZ + 3*HIST_rCHECKSUM_SZ, HIST_rLIVE_RPM_SZ + HIST_rCHECKSUM_SZ)) {
        ret++;
    }

    if (ret) {
        v_level = verbose;
    }

    if (v_level == verbose ) {
        uart0_print("RX ");
        for (i=0; i<8; i++) {
            snprintf(str_temp, STR_LEN, "%02x%02x%02x%02x ", *(data+(i*4)), *(data+(i*4)+1), *(data+(i*4)+2), *(data+(i*4)+3));
            uart0_print(str_temp);
        }
        uart0_print("\r\n");
        snprintf(str_temp, STR_LEN, "st:   0x%02x %s%s%s%s%s%s", data[0], (data[0] & HIST_bmERR4) ? " ERR4" : "",
            (data[0] & HIST_bmERR2) ? " ERR2" : "", (data[0] & HIST_bmERR1) ? " ERR1" : "",
            (data[0] & HIST_bmBUSY) ? " BSY" : "", (data[0] & HIST_bmADJ) ? " ADJ" : "",
            SPIVerifyCheckSum(data, 2) ? " chk ok\r\n" : "chk fail\r\n");
        uart0_print(str_temp);
        snprintf(str_temp, STR_LEN, "set:  %d rpm, %s", (int16_t) ((data[HIST_rTARGET_RPM + 1] << 8) + data[HIST_rTARGET_RPM]), SPIVerifyCheckSum(data + HIST_rTARGET_RPM, HIST_rTARGET_RPM_SZ + HIST_rCHECKSUM_SZ) ? "chk ok\r\n" : "chk fail\r\n");
        uart0_print(str_temp);
        snprintf(str_temp, STR_LEN, "live: %d rpm, %s", (int16_t) ((data[HIST_rLIVE_RPM + 1] << 8) + data[HIST_rLIVE_RPM]), SPIVerifyCheckSum(data + HIST_rLIVE_RPM, HIST_rLIVE_RPM_SZ + HIST_rCHECKSUM_SZ) ? "chk ok\r\n" : "chk fail\r\n");
        uart0_print(str_temp);
        uart0_print("\r\n");
    }

    return ret;
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    uint16_t i;
    int32_t in;
    uint32_t err_cnt = 0;
    uint32_t ret = 0;

    if (f == '?') {
        display_menu();
    } else if (strstr(input, "info")) {
        snprintf(str_temp, STR_LEN, "UCB1CTLW0 0x%x, UCB1BRW 0x%x\r\n", UCB1CTLW0, UCB1BRW);
        uart0_print(str_temp);
    } else if (strstr(input, "fact")) {
        memset(&data, 0, 32);
        data[0] = 0xeb;
        hist_write_reg(EUSCI_SPI_BASE_ADDR, HIST_rPASSWD, &data[0], 1);
        memset(&data, 0, 32);
        memcpy(&data, def_serial_no, HIST_rSERIAL_NO_SZ);
        data[12] = SPIComputeCheckSum(data, HIST_rSERIAL_NO_SZ);
        hist_write_reg(EUSCI_SPI_BASE_ADDR, HIST_rSERIAL_NO, &data[0], HIST_rSERIAL_NO_SZ + HIST_rCHECKSUM_SZ);
    } else if (strstr(input, "rst")) {
        // panic - send RST to the uC
        memset(&data, 0, 32);
        data[0] = HIST_bmRST;
        data[1] = SPIComputeCheckSum(data, 1);
        hist_write_reg(EUSCI_SPI_BASE_ADDR, HIST_rMODE, &data[0], 2);
    } else if (strstr(input, "clr")) {
        memset(&data, 0, 32);
        data[0] = HIST_bmCLR;
        data[1] = SPIComputeCheckSum(data, 1);
        hist_write_reg(EUSCI_SPI_BASE_ADDR, HIST_rMODE, &data[0], 2);
    } else if (strstr(input, "t1")) {
        for (i=0;i<10000;i++) {
            err_cnt += get_status(quiet);
        }
        snprintf(str_temp, STR_LEN, "t1 %lu errors\r\n", err_cnt);
        uart0_print(str_temp);
    } else if (strstr(input, "t2")) {
        for (i=0;i<1000;i++) {
            ret = set_and_check_speed(quiet, (15+i%3)*100);
            if (ret != 0) {
                snprintf(str_temp, STR_LEN, " task %u \r\n", i);
                uart0_print(str_temp);
            }
            err_cnt += ret;
        }
        snprintf(str_temp, STR_LEN, "t2 %lu errors\r\n", err_cnt);
        uart0_print(str_temp);
        set_and_check_speed(quiet, 0);
    } else if (strstr(input, "t3")) {
        for (i=0;i<1000;i++) {
            err_cnt += read_t3(quiet);
        }
        snprintf(str_temp, STR_LEN, "t3 %lu errors\r\n", err_cnt);
        uart0_print(str_temp);
    } else if (f == 'r') {
        memset(&data, 0, 32);
        hist_read_reg(EUSCI_SPI_BASE_ADDR, 0, &data[0], 32);

        uart0_print("RX ");
        for (i=0; i<8; i++) {
            snprintf(str_temp, STR_LEN, "%02x%02x%02x%02x ", *(data+(i*4)), *(data+(i*4)+1), *(data+(i*4)+2), *(data+(i*4)+3));
            uart0_print(str_temp);
        }
        uart0_print("\r\n");
    } else if (f == 'g') {
        get_status(verbose);
    } else if (f == 's') {
        if (str_to_int32(input, &in, 1, strlen(input) - 1, -2500, 2500) == EXIT_FAILURE) {
            uart0_print("error during str_to_uint32()\r\n");
            return;
        }
        set_and_check_speed(verbose, in);
    } else {
        uart0_print("\r\n");
    }
}

