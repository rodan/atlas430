
//  author:          Petre Rodan <2b4eda@subdimension.ro>
//  available from:  https://github.com/rodan/atlas430fr5x
//  license:         GNU GPLv3

/**
 * \file
 *         Header file for the mathematics library
 * \author
 *         Petre Rodan <2b4eda@subdimension.ro>
 */

/**
* \addtogroup Library
* @{ */

/**
 * \defgroup lib_math Mathematics library
 * @{
  simplified functions for trigonometric calculations

  to be used when either the math functions are missing from libc
  or when space is at a premium
**/

#ifndef __LIB_MATH_H__
#define __LIB_MATH_H__

#define                  PI  3.14159265358  ///< pi used by _atan2f()
#define            PI_FLOAT  3.14159265f    ///< float pi used by _atan2f()
#define         PIBY2_FLOAT  1.5707963f     ///< half pi used by _atan2f()
#define                PREC  5              ///< precision used by _sin() and _cos()

#ifdef __cplusplus
extern "C" {
#endif

/** calculate arc tangent function of two variables
    @param x first variable
    @param y second variable
    @return calculated value
*/
float _atan2f(float y, float x);

/** calculate sine of an input
    @param x input value (float)
    @return sin(input)
*/
float _sin(const float x);

/** calculate the cosine of an input
    @param x input value (float)
    @return sin(input) (float)
*/
float _cos(const float x);

/** calulate the square root of an input
    @param x input value (float)
    @return sqrt(input) (float)
*/
float _sqrt(const float x);

/** convert degrees to radians
    @param x degree value to be converted (float)
    @return resulting radians (float)
*/
float radians(const float x);

/** return the square of a float
    @param x input value to be squared (float)
    @return input^2 (float)
*/
float sq(const float x);

#ifdef __cplusplus
}
#endif

#endif

/** @}*/
/** @}*/
