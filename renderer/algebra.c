#include "algebra.h"
#include <math.h>

//
// 2-dimensional vector
//
xy_t xy(real_t x, real_t y)
{
    xy_t xy = { x, y };
    return xy;
}

xy_t xy_add(xy_t a, xy_t b)
{
    xy_t res = {
        a.x + b.x,
        a.y + b.y
    };
    return res;
}

xy_t xy_sub(xy_t a, xy_t b)
{
    xy_t res = {
        a.x - b.x,
        a.y - b.y
    };
    return res;
}

xy_t xy_scale(xy_t a, real_t f)
{
    xy_t res = {
        a.x * f,
        a.y * f
    };
    return res;
}

xy_t xy_invScale(xy_t a, real_t f)
{
    return xy_scale(a, real_reciprocal(f));
}

real_t xy_cross(xy_t a, xy_t b)
{
    return a.x * b.y - a.y * b.x;
}

real_t xy_dot(xy_t a, xy_t b)
{
    return a.x * b.x + a.y * b.y;
}

xy_t xy_orthogonal(xy_t a)
{
    xy_t res = { -a.y, a.x };
    return res;
}

xz_t xz(real_t x, real_t z)
{
    xz_t xz = { x, z };
    return xz;
}

#define xy_to_xz(xy) (xz(xy.x, xy.y))
#define xz_to_xy(xz) (xy(xz.x, xz.z))

xz_t xz_add(xz_t a, xz_t b)
{
    return xy_to_xz(xy_add(xz_to_xy(a), xz_to_xy(b)));
}

xz_t xz_sub(xz_t a, xz_t b)
{
    return xy_to_xz(xy_sub(xz_to_xy(a), xz_to_xy(b)));
}

xz_t xz_scale(xz_t a, real_t f)
{
    return xy_to_xz(xy_scale(xz_to_xy(a), f));
}

xz_t xz_invScale(xz_t a, real_t f)
{
    return xy_to_xz(xy_invScale(xz_to_xy(a), f));
}

real_t xz_cross(xz_t a, xz_t b)
{
    return xy_cross(xz_to_xy(a), xz_to_xy(b));
}

real_t xz_dot(xz_t a, xz_t b)
{
    return xy_dot(xz_to_xy(a), xz_to_xy(b));
}

xz_t xz_orthogonal(xz_t a)
{
    return xy_to_xz(xy_orthogonal(xz_to_xy(a)));
}

//
// line-intersection
//
bool_t xz_lineIntersect(lineSeg_t seg1, lineSeg_t seg2, xz_t* result)
{
    xz_t seg1Vector = xz_sub(seg1.start.xz, seg1.end.xz);
    xz_t seg2Vector = xz_sub(seg2.start.xz, seg2.end.xz);
    real_t det = xz_cross(seg1Vector, seg2Vector);
    if (real_compare(det, real_zero) == 0)
        return false;
    real_t seg1Det = xz_cross(seg1.start.xz, seg1.end.xz);
    real_t seg2Det = xz_cross(seg2.start.xz, seg2.end.xz);
    real_t ix = xz_cross(xz(seg1Det, seg1Vector.x), xz(seg2Det, seg2Vector.x));
    real_t iz = xz_cross(xz(seg1Det, seg1Vector.z), xz(seg2Det, seg2Vector.z));
    result->x = real_div(ix, det);
    result->z = real_div(iz, det);
    return true;
}

real_t xz_linePhase(lineSeg_t seg, xz_t intersection)
{
    xz_t segVector = xz_sub(seg.start.xz, seg.end.xz);
    return real_compare(segVector.x, real_zero) != 0
        ? real_div(real_sub(seg.start.xz.x, intersection.x), segVector.x)
        : real_div(real_sub(seg.start.xz.z, intersection.z), segVector.z);
}

bool_t xy_lineIntersect(lineSeg_t seg1, lineSeg_t seg2, xy_t* result)
{
    return xz_lineIntersect(seg1, seg2, (xz_t*)result);
}

real_t xy_linePhase(lineSeg_t seg, xy_t intersection)
{
    return xz_linePhase(seg, xy_to_xz(intersection));
}
