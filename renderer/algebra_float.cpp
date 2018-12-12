#include "algebra.hpp"
#include <stdio.h>
#include <math.h>

Real::Real(int i): v((float)i) { }
Real::Real(float f): v((float)f) { }

int Real::asInt() const { return (int) v; }
float Real::asFloat() const { return v; }
int Real::asString(char* str, int maxlen) const { return snprintf(str, maxlen, "%f", v); }

Real Real::operator + (Real r) const { return Real(v + r.v); }
Real Real::operator - (Real r) const { return Real(v - r.v); }
Real Real::operator * (Real r) const { return Real(v * r.v); }
Real Real::operator / (Real r) const { return Real(v / r.v); }
Real Real::operator - () const { return Real(-v); }
bool Real::operator < (Real b) const { return v < b.v; }
bool Real::operator > (Real b) const { return v > b.v; }
bool Real::operator <= (Real b) const { return v <= b.v; }
bool Real::operator >= (Real b) const { return v >= b.v; }
bool Real::operator == (Real b) const { return v == b.v; }
bool Real::operator != (Real b) const { return v != b.v; }

Real Real::reciprocal() const { return Real(1.0f / v); }
Real Real::sin() const { return Real(sinf(v)); }
Real Real::cos() const { return Real(cosf(v)); }
Real Real::tan() const { return Real(tanf(v)); }
Real Real::toRad() const { return Real(v * pi.v / 180.0f); }
Real Real::toDeg() const { return Real(v / pi.v * 180.0f); }
Real Real::floor() const { return Real(floorf(v)); }
Real Real::ceil() const { return Real(ceilf(v)); }
Real Real::round() const { return Real(roundf(v)); }
Real Real::sqrt() const { return Real(sqrtf(v)); }
Real Real::invSqrt() const { return sqrt().reciprocal(); }

int Real::compare(Real b) const {
    return (*this > b) - (*this < b);
}
bool Real::inBetween(Real min, Real max) const {
    return *this >= min && *this <= max;
}

Real Real::lerp(Real start, Real end) const {
    return *this * (end - start) + start;
}

Real Real::lerp(Real inputStart, Real inputEnd, Real outputStart, Real outputEnd) const {
    Real normalized = (*this - inputStart) / (inputEnd - inputStart);
    return normalized.lerp(outputStart, outputEnd);
}

Real Real::clamp(Real minimum, Real maximum) const {
    return max(minimum, min(*this, maximum));
}

Real Real::fractional() const {
    float dummy;
    return Real(modf(v, &dummy));
}

const Real Real::one = Real(1);
const Real Real::zero = Real(0);
const Real Real::pi = Real(3.141592653f);
const Real Real::rotationLeft = Real(-1);
const Real Real::rotationRight = Real(1);
