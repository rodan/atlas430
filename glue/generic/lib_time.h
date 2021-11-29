
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/atlas430fr5x
//  license:         GNU GPLv3

/**
 * \file
 *         Header file for the time and date conversion library
 * \author
 *         Petre Rodan <2b4eda@subdimension.ro>
 */

/**
* \addtogroup Library
* @{ */

/**
 * \defgroup lib_time time and date conversion library
 * @{
  simple functions for converting time and date data between different formats

  to be used when the libc implementation takes too much space
**/

#ifndef __LIB_TIME_H__
#define __LIB_TIME_H__

#include <time.h>               // for struct tm

struct ts {
    uint8_t sec;                ///< seconds
    uint8_t min;                ///< minutes
    uint8_t hour;               ///< hours
    uint8_t mday;               ///< day of the month
    uint8_t mon;                ///< month
    int16_t year;               ///< year
    uint8_t wday;               ///< day of the week [1-7] 1 == monday, 7 == sunday
    uint8_t yday;               ///< day in the year
    uint8_t isdst;              ///< daylight saving time
    uint8_t year_s;             ///< year in short notation
#ifdef CONFIG_UNIXTIME
    uint64_t unixtime;          ///< seconds since 01.01.1970 00:00:00 UTC
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

/** return the number of seconds since 01.01.1970 00:00:00 UTC
    @param t input ts time structure
    @return calculated unixtime 
*/
uint64_t get_unixtime(struct ts t);

/** function that converts the number of seconds since January 1st 2000 into a tm structure
    @param sec_since_2000 input time_t (uint32_t) seconds since January 1st 2000
    @param p_time input the converted tm struct
    @return void
*/
void _gmtime(time_t sec_since_2000, struct tm *p_time);

#ifdef __cplusplus
}
#endif

#endif

/** @}*/
/** @}*/
