
#include <inttypes.h>
#include <math.h>
#include "lib_math.h"

// |error| < 0.005
float _atan2f(const float y, const float x)
{

    float rv;
    float z = y / x;

    if (x == 0.0f) {
        if (y > 0.0f) {
            return PIBY2_FLOAT;
        }
        if (y == 0.0f) {
            return 0.0f;
        }
        return -PIBY2_FLOAT;
    }
    if (fabsf(z) < 1.0f) {
        rv = z / (1.0f + 0.28f * z * z);
        if (x < 0.0f) {
            if (y < 0.0f) {
                return rv - PI_FLOAT;
            }
            return rv + PI_FLOAT;
        }
    } else {
        rv = PIBY2_FLOAT - z / (z * z + 0.28f);
        if (y < 0.0f) {
            return rv - PI_FLOAT;
        }
    }
    return rv;
}

float _sin(const float x)
{
    unsigned char i;
    float denum = 1;
    float res = 0;
    float x_2 = x * x;
    float num = x;
    int s = 1;
    for (i = 0; i < PREC; i++) {
        res += s * (num / denum);
        denum = denum * (denum + 1) * (denum + 2);
        num = num * x_2;
        s = s * -1;
    }
    return res;
}

float _cos(const float x)
{
    unsigned char i;
    float denum = 2;
    float res = 1;
    float x_2 = x * x;
    float num = x_2;
    int s = -1;
    for (i = 0; i < PREC; i++) {
        res += s * (num / denum);
        denum = denum * (denum + 1) * (denum + 2);
        num = num * x_2;
        s = s * -1;
    }
    return res;
}

float _sqrt(const float number)
{
    unsigned char i = 0;
    float x0, sqx0, error;
    if (number < 1) {
        x0 = number * 2;
    } else {
        x0 = number / 2;
    }
    do {
        x0 = (x0 + (number / x0)) / 2;
        sqx0 = x0 * x0;
        error = (number - sqx0) / number;
        i++;
    } while (i < 20 && ((error > 0 && error > 0.01) || (error < 0 && error < -0.01)));

    return x0;
}

float radians(const float x)
{
    return PI * x / 180.0f;
}

float sq(const float x)
{
    return x * x;
}

