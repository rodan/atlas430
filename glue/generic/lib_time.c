
#include <inttypes.h>
//#include <stdlib.h>
//#include <time.h>
#include "lib_time.h"

#define SECONDS_FROM_1970_TO_2000 946684800 ///< precalculated value of seconds between 1st of Jan 1970 UTC until 1st of Jan 2000 UTC

uint64_t get_unixtime(struct ts t)
{
    const uint8_t days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    uint8_t i;
    uint16_t d;
    int16_t y;
    uint64_t rv;

    if (t.year >= 2000) {
        y = t.year - 2000;
    } else {
        return 0;
    }

    d = t.mday - 1;
    for (i = 1; i < t.mon; i++) {
        d += (uint16_t) (days_in_month[i - 1]);
    }
    if (t.mon > 2 && y % 4 == 0) {
        d++;
    }
    // count leap days
    d += (365 * y + (y + 3) / 4);
    rv = ((d * 24UL + t.hour) * 60 + t.min) * 60 + t.sec + SECONDS_FROM_1970_TO_2000;
    return rv;
}

void _gmtime(time_t sec_since_2000, struct tm *p_time)
{
    const uint8_t MonthDays[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    uint32_t tmp1, tmp2;
    uint16_t iBuf;
    uint8_t i;
    uint8_t leap;

    tmp1 = sec_since_2000 / 0x00015180; //days

    tmp2 = sec_since_2000 - tmp1 * 0x00015180;
    p_time->tm_hour = tmp2 / 3600;      //hours

    tmp1 = tmp2 - ((uint32_t) p_time->tm_hour) * 3600;
    p_time->tm_min = tmp1 / 60; //minute
    p_time->tm_sec = tmp1 - (uint32_t) p_time->tm_min * 60;     //second
    //we have the rest of the information in tmp  ; get day, month and year

    iBuf = sec_since_2000 / 0x00015180;
    for (i = 0; i < 100; i++) {
        leap = (i % 4 == 0);
        if (iBuf < (365 + leap))
            break;
        iBuf -= (365 + leap);
    }
    p_time->tm_year = i;        //year
    leap = (i % 4 == 0);
    for (i = 1;; i++) {
        if (iBuf < (MonthDays[i] + (i == 2 && leap)))
            break;
        iBuf -= MonthDays[i] + (i == 2 && leap);
    }
    p_time->tm_mon = i;         //month
    p_time->tm_mday = iBuf + 1; //day
}

