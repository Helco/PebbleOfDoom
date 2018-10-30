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
