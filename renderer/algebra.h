#ifndef ALGEBRA_H
#define ALGEBRA_H

//
// Integer
//
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

int lerpi(int value, int inputStart, int inputEnd, int outputStart, int outputEnd);
int clampi(int start, int value, int end);

//
// Boolean
//
typedef int bool_t;
#define true 1
#define false 0

//
// Real
//

#if defined REAL_USE_FLOAT
typedef struct { float v; } real_t;
#else
#error You did not specify a real_t implementation
#endif

extern const real_t
    real_one,
    real_zero,
    real_pi,
    real_2pi;

real_t real_from_int(int i);
int real_to_int(real_t a);
real_t real_from_float(float f);
float real_to_float(real_t a);
int real_to_string(real_t a, char* str, int maxlen); // returns number of characters written

real_t real_add(real_t a, real_t b);
real_t real_sub(real_t a, real_t b);
real_t real_mul(real_t a, real_t b);
real_t real_div(real_t a, real_t b);
real_t real_reciprocal(real_t a);
real_t real_neg(real_t a);

real_t real_sin(real_t a); // in radians
real_t real_cos(real_t a);
real_t real_tan(real_t a);
real_t real_degToRad(real_t deg);
real_t real_radToDeg(real_t rad);

int real_compare(real_t a, real_t b);
bool_t real_inBetween(real_t value, real_t min, real_t max);
real_t real_min(real_t a, real_t b);
real_t real_max(real_t a, real_t b);
real_t real_abs(real_t a);
int real_signInt(real_t a);
real_t real_lerp(real_t value, real_t start, real_t end); // input amplitude is normal 1
real_t real_clamp(real_t minimum, real_t value, real_t maximum);

real_t real_floor(real_t a);
real_t real_ceil(real_t a);
real_t real_round(real_t a);
real_t real_fractional(real_t a);
real_t real_sqrt(real_t a);
real_t real_invSqrt(real_t a);

//
// 2-dimensional Vector
//
typedef struct xz_t {
    real_t x, z;
} xz_t;

typedef struct xy_t {
    real_t x, y;
} xy_t;

#define xy_one (xy(real_one, real_one))
#define xy_zero (xy(real_zero, real_zero))
#define xy_forward (xy(real_one, real_zero))
#define xy_backward (xy(real_neg(real_one), real_zero))
#define xy_up (xy(real_zero, real_one))
#define xy_down (xy(real_zero, real_neg(real_one)))
xy_t xy(real_t x, real_t y);
xy_t xy_add(xy_t a, xy_t b);
xy_t xy_sub(xy_t a, xy_t b);
xy_t xy_scale(xy_t a, real_t f);
xy_t xy_invScale(xy_t a, real_t f);
real_t xy_cross(xy_t a, xy_t b);
real_t xy_dot(xy_t a, xy_t b);
xy_t xy_orthogonal(xy_t a);
real_t xy_lengthSqr(xy_t x);
real_t xy_length(xy_t x);
xy_t xy_normalize(xy_t x);
xy_t xy_rotate(xy_t a, real_t angleInRad);
xy_t xy_neg(xy_t a);

#define xz_one (xz(real_one, real_one))
#define xz_zero (xz(real_zero, real_zero))
#define xz_right (xz(real_zero, real_one))
#define xz_left (xz(real_zero, real_neg(real_one)))
#define xz_forward (xz(real_one, real_zero))
#define xz_backward (xz(real_neg(real_one), real_zero))
xz_t xz(real_t x, real_t z);
xz_t xz_add(xz_t a, xz_t b);
xz_t xz_sub(xz_t a, xz_t b);
xz_t xz_scale(xz_t a, real_t f);
xz_t xz_invScale(xz_t a, real_t f);
real_t xz_cross(xz_t a, xz_t b);
real_t xz_dot(xz_t a, xz_t b);
xz_t xz_orthogonal(xz_t a);
real_t xz_lengthSqr(xz_t x);
real_t xz_length(xz_t x);
xz_t xz_normalize(xz_t x);
xz_t xz_rotate(xz_t a, real_t angleInRad);
xz_t xz_neg(xz_t a);

//
// line intersection
//
typedef struct lineSeg_t {
    union {
        xy_t xy;
        xz_t xz;
    } start;
    union {
        xy_t xy;
        xz_t xz;
    } end;
} lineSeg_t;

bool_t xz_lineIntersect(lineSeg_t seg1, lineSeg_t seg2, xz_t* result);
real_t xz_linePhase(lineSeg_t seg, xz_t intersection);
bool_t xy_lineIntersect(lineSeg_t seg1, lineSeg_t seg2, xy_t* result);
real_t xy_linePhase(lineSeg_t seg, xy_t intersection);


#endif
