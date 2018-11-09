#include "algebra.h"

#undef abs // this conflicts with math.h
#undef min
#undef max

#ifdef REAL_USE_FLOAT
#include <math.h>
#include <pebble.h>

const real_t real_one = 1.0f;
const real_t real_zero = 0.0f;
#define REAL_PI 3.141592653f
const real_t real_pi = REAL_PI;
const real_t real_2pi = 2 * REAL_PI;

real_t real_from_int(int i)
{
    return (real_t)i;
}

int real_to_int(real_t a)
{
    return (int)a;
}

real_t real_from_float(float f)
{
    return (real_t)f;
}

float real_to_float(real_t a)
{
    return (float)a;
}

int real_to_string(real_t a, char* str, int maxlen)
{
    return snprintf(str, maxlen, "%f", a);
}

real_t real_add(real_t a, real_t b)
{
    return a + b;
}

real_t real_sub(real_t a, real_t b)
{
    return a - b;
}

real_t real_mul(real_t a, real_t b)
{
    return a * b;
}

real_t real_div(real_t a, real_t b)
{
    return a / b;
}

real_t real_reciprocal(real_t a)
{
    return 1.0f / a;
}

real_t real_neg(real_t a)
{
    return -a;
}

real_t real_sin(real_t a)
{
    float s = (float)sin_lookup((int)(a * TRIG_MAX_ANGLE / real_2pi));
    return (s / TRIG_MAX_RATIO);
}

real_t real_cos(real_t a)
{
    float s = (float)cos_lookup((int)(a * TRIG_MAX_ANGLE / real_2pi));
    return (s / TRIG_MAX_RATIO);
}

real_t real_tan(real_t a)
{
    return real_sin(a) / real_cos(a);
}

real_t real_degToRad(real_t deg)
{
    return deg * real_pi / 180.0f;
}

real_t real_radToDeg(real_t rad)
{
    return rad * 180.0f / real_pi;
}

int real_compare(real_t a, real_t b)
{
    return (a > b) - (a < b);
}

bool_t real_inBetween(real_t value, real_t min, real_t max)
{
    return value >= min && value <= max;
}

real_t real_min(real_t a, real_t b)
{
    return a < b ? a : b;
}

real_t real_max(real_t a, real_t b)
{
    return a > b ? a : b;
}

real_t real_abs(real_t a)
{
    return a < 0 ? -a : a;
}

int real_signInt(real_t a)
{
    return a < 0 ? -1 : 1;
}

real_t real_floor(real_t a)
{
    return floorf(a);
}

real_t real_ceil(real_t a)
{
    return ceilf(a);
}

real_t real_round(real_t a)
{
    return roundf(a);
}

real_t real_fractional(real_t a)
{
    float dummy;
    return modff(a, &dummy);
}

real_t real_sqrt(real_t a)
{
    return sqrtf(a);
}

real_t real_invSqrt(real_t a)
{
    return 1.0f / sqrtf(a);
}

#endif
