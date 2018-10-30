#ifndef ALGEBRA_H
#define ALGEBRA_H

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
typedef float real_t;
#else
#error You did not specify a real_t implementation
#endif

extern real_t real_one, real_zero;

real_t real_from_int(int i);
real_t real_add(real_t a, real_t b);
real_t real_sub(real_t a, real_t b);
real_t real_mul(real_t a, real_t b);
real_t real_div(real_t a, real_t b);
real_t real_reciprocal(real_t a);

real_t real_sin(real_t a); // in radians
real_t real_cos(real_t a);
real_t real_tan(real_t a);

int real_compare(real_t a, real_t b);
real_t real_min(real_t a, real_t b);
real_t real_max(real_t a, real_t b);
real_t real_abs(real_t a);

//
// 2-dimensional Vector
//
typedef struct xz_t {
    real_t x, z;
} xz_t;

typedef struct xy_t {
    real_t x, y;
} xy_t;

xy_t xy(real_t x, real_t y);
xy_t xy_add(xy_t a, xy_t b);
xy_t xy_sub(xy_t a, xy_t b);
xy_t xy_scale(xy_t a, real_t f);
xy_t xy_invScale(xy_t a, real_t f);
real_t xy_cross(xy_t a, xy_t b);
real_t xy_dot(xy_t a, xy_t b);
xy_t xy_orthogonal(xy_t a);

xz_t xz(real_t x, real_t z);
xz_t xz_add(xz_t a, xz_t b);
xz_t xz_sub(xz_t a, xz_t b);
xz_t xz_scale(xz_t a, real_t f);
xz_t xz_invScale(xz_t a, real_t f);
real_t xz_cross(xz_t a, xz_t b);
real_t xz_dot(xz_t a, xz_t b);
xz_t xz_orthogonal(xz_t a);

//
// line intersection
//
typedef struct lineIntersection_t {
    union {
        xy_t xy;
        xz_t xz;
    };
    real_t phase1;
    real_t phase2;
} lineIntersection_t;

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

bool_t xz_intersect(lineSeg_t seg1, lineSeg_t seg2, lineIntersection_t* result);
#define xy_intersect xz_intersect

#endif
