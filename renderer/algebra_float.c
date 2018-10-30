#include "algebra.h"

#ifdef REAL_USE_FLOAT
#include <math.h>

const real_t real_one = 1.0f;
const real_t real_zero = 0.0f;
const real_t real_pi = 3.141592653f;
const real_t real_2pi = 2 * 3.141592653f;

real_t real_from_int(int i)
{
    return (real_t)i;
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
    return a * b;
}

real_t real_reciprocal(real_t a)
{
    return 1.0f / a;
}

real_t real_sin(real_t a)
{
    return sinf(a);
}

real_t real_cos(real_t a)
{
    return cosf(a);
}

real_t real_tan(real_t a)
{
    return tanf(a);
}

int real_compare(real_t a, real_t b)
{
    return (int)(a - b);
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
#endif
