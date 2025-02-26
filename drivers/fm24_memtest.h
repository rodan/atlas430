#ifndef __FM24_MEMTEST_H__
#define __FM24_MEMTEST_H__

#include <inttypes.h>
#include "bus.h"

typedef enum {
    TEST_00,
    TEST_FF,
    TEST_AA,
    TEST_CNT
} FM24_test_t;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t fm24_memtest(device_t *dev, const uint32_t start_addr, const uint32_t stop_addr, FM24_test_t test, uint32_t *rows_tested);

#ifdef __cplusplus
}
#endif

#endif
