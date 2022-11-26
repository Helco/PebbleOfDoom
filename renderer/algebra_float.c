#include "algebra.h"

#undef abs // this conflicts with math.h
#undef min
#undef max

#ifdef REAL_USE_FLOAT
#include <math.h>
#include <pebble.h>

const real_t real_one = { 1.0f };
const real_t real_zero = { 0.0f };
#define REAL_PI 3.141592653f
const real_t real_pi = { REAL_PI };
const real_t real_2pi = { 2 * REAL_PI };

real_t real_from_int(int i)
{
    return (real_t) { (float)i };
}

int real_to_int(real_t a)
{
    return (int)a.v;
}

real_t real_from_float(float f)
{
    return (real_t) { f };
}

float real_to_float(real_t a)
{
    return (float)a.v;
}

int real_to_string(real_t a, char* str, int maxlen)
{
    return snprintf(str, maxlen, "%f", a.v);
}

real_t real_add(real_t a, real_t b)
{
    return (real_t){ a.v + b.v };
}

real_t real_sub(real_t a, real_t b)
{
    return (real_t){ a.v - b.v };
}

real_t real_mul(real_t a, real_t b)
{
    return (real_t){ a.v * b.v };
}

real_t real_div(real_t a, real_t b)
{
    return (real_t){ a.v / b.v };
}

real_t real_reciprocal(real_t a)
{
    return (real_t){ 1.0f / a.v };
}

real_t real_neg(real_t a)
{
    return (real_t){ -a.v };
}

real_t real_sin(real_t a)
{
    float s = (float)sin_lookup((int)(a.v * TRIG_MAX_ANGLE / real_2pi.v));
    return (real_t){ s / TRIG_MAX_RATIO };
}

real_t real_cos(real_t a)
{
    float s = (float)cos_lookup((int)(a.v * TRIG_MAX_ANGLE / real_2pi.v));
    return (real_t){ s / TRIG_MAX_RATIO };
}

real_t real_tan(real_t a)
{
    return (real_t){ real_sin(a).v / real_cos(a).v };
}

real_t real_degToRad(real_t deg)
{
    return (real_t){ deg.v * real_pi.v / 180.0f };
}

real_t real_radToDeg(real_t rad)
{
    return (real_t){ rad.v * 180.0f / real_pi.v };
}

int real_compare(real_t a, real_t b)
{
    return (a.v > b.v) - (a.v < b.v);
}

bool_t real_inBetween(real_t value, real_t min, real_t max)
{
    return value.v >= min.v && value.v <= max.v;
}

real_t real_min(real_t a, real_t b)
{
    return (real_t){ a.v < b.v ? a.v : b.v };
}

real_t real_max(real_t a, real_t b)
{
    return (real_t){ a.v > b.v ? a.v : b.v };
}

real_t real_abs(real_t a)
{
    return (real_t){ a.v < 0 ? -a.v : a.v };
}

int real_signInt(real_t a)
{
    return a.v < 0 ? -1 : 1;
}

real_t real_lerp(real_t value, real_t start, real_t end)
{
    const real_t ampl = real_sub(end, start);
    return real_add(real_mul(value, ampl), start);
}

real_t real_norm_lerp(real_t value, real_t inputStart, real_t inputEnd, real_t outputStart, real_t outputEnd)
{
    real_t normalized = real_div(real_sub(value, inputStart), real_sub(inputEnd, inputStart));
    return real_lerp(normalized, outputStart, outputEnd);
}

real_t real_clamp(real_t minimum, real_t value, real_t maximum)
{
    return real_max(minimum, real_min(value, maximum));
}

real_t real_floor(real_t a)
{
    return (real_t){ floorf(a.v) };
}

real_t real_ceil(real_t a)
{
    return (real_t){ ceilf(a.v) };
}

real_t real_round(real_t a)
{
    return (real_t){ roundf(a.v) };
}

real_t real_fractional(real_t a)
{
    float dummy;
    return (real_t){ modff(a.v, &dummy) };
}

/*
real_t real_sqrt(real_t a)
{
    return (real_t){ sqrtf(a.v) };
}

real_t real_invSqrt(real_t a)
{
    return (real_t){ 1.0f / sqrtf(a.v) };
}*/

real_t real_invSqrt(real_t a)
{
    union {
        float    f;
        uint32_t i;
    } conv = { .f = a.v };
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5F - (a.v * 0.5F * conv.f * conv.f);
    return (real_t) { conv.f };
}

real_t real_sqrt(real_t a)
{
    return (real_t) { 1.0f / real_invSqrt(a).v };
}

#endif
