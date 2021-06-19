#ifndef __QA_H__
#define __QA_H__

#include "proj.h"
//#include "drivers/fm24_memtest.h"

/*
void display_memtest(const uint32_t start_addr, const uint32_t stop_addr, fm24_test_t test);
*/

void display_menu(void);

void parse_user_input(void);
void parse_sensor_output(void);
void print_buf(uint8_t * data, const uint16_t size);
void qa_task(void);

#endif
