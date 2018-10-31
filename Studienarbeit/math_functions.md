# Math funtions

# Boolean
There should exists a boolean value. Focus lies on speed not on memory.

```
bool_t a;
a = true;
a = false;
if (a) { puts("Is true"); }
else { puts("Is false"); }
```

# Real

Real is a not-further-specified datatype to store numbers with both integer and fractional parts, according to the following interface.
This interface is to be implemented with several variants (float, fixed-point-64, fixed-point-32, etc.) with reciprocal usage in division.
Also it should warn on not-pebble-hardware platforms when information is lost.

```
extern real_t real_one, real_zero;
extern real_t real_pi, real_2pi;

real_t real_from_int(int a);
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

int real_to_int(real_t a);
int real_to_string(real_t a, char* str, int maxlen); // returns number of characters written
real_t real_floor(real_t a);
real_t real_ceil(real_t a);
real_t real_round(real_t a);
real_t real_fractional(real_t a);
real_t real_sqrt(real_t a);
real_t real_invSqrt(real_t a); // reciprocal of sqrt, may be faster to calculate than 1 / sqrt
// no real_integer as this is equivalent to real_floor
```

# Vector 2-dimensional

The vector type should use the real type (see above).

```
struct xz_t {
    real_t x, z;
};
struct xy_t {
    real_t x, y;
};

xy_t xy(real_t x, real_t y);
xz_t xz(real_t x, real_t z);
xy_t xy_add(xy_t a, xy_t b);
xz_t xz_add(xz_t a, xz_t b); // the following equaivalent functions were skipped
xy_t xy_sub(xy_t a, xy_t b);
xy_t xy_scale(xy_t a, real_t f);
xy_t xy_invScale(xy_t a, real_t f);
real_t xy_cross(xy_t a, xy_t b);
real_t xy_dot(xy_t a, xy_t b);
xy_t xy_orthogonal(xy_t a);
real_t xy_lengthSqr(xy_t x); // sometimes only |v|^2 is needed
real_t xy_length(xy_t x);
xy_t xy_normalize(xy_t x);
```

# Line-intersection

```
struct LineIntersection_t
{
    union
    {
        xy_t xy;
        xz_t xz;
    };
    real_t phase;
};

struct LineSeg_t {
    union {
        xy_t xy;
        xz_t xz;
    } start;
    union {
        xy_t xy;
        xz_t xz;
    } end;
}

bool_t xz_intersect(LineSeg_t seg1, LineSeg_t seg2, LineIntersection_t* result);
bool_t xy_intersect(LineSeg_t seg1, LineSeg_t seg2, LineIntersection_t* result);
```
