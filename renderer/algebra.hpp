#ifndef ALGEBRA_HPP
#define ALGEBRA_HPP
#include "platform.h"

//
// Integer
//

template<typename T> inline T min(T a, T b) {
    return a < b ? a : b;
}

template<typename T> inline T max(T a, T b) {
    return a > b ? a : b;
}

int lerpi(int value, int inputStart, int inputEnd, int outputStart, int outputEnd);
int clampi(int start, int value, int end);

//
// Real
//
namespace internal {
    template<class T> class CommonAssignArithmetic {
    public:
        T& operator += (const T b) {
            T& me = static_cast<T&>(*this);
            return (me = me + b);
        }
        T& operator -= (const T b) {
            T& me = static_cast<T&>(*this);
            return (me = me - b);
        }
        T& operator *= (const T b) {
            T& me = static_cast<T&>(*this);
            return (me = me * b);
        }
        T& operator /= (const T b) {
            T& me = static_cast<T&>(*this);
            return (me = me / b);
        }
    };
}

class Real : public internal::CommonAssignArithmetic<Real> {
    #if defined REAL_USE_FLOAT
    float v;
    #else
    #error You did not specify a real_t implementation
    #endif
public:
    explicit Real(int i);
    explicit Real(float f);

    int asInt() const;
    float asFloat() const;
    int asString(char* str, int maxlen) const;

    Real operator + (Real r) const;
    Real operator - (Real r) const;
    Real operator * (Real r) const;
    Real operator / (Real r) const;

    Real operator -() const;
    Real reciprocal() const;

    Real sin() const;
    Real cos() const;
    Real tan() const;
    Real toRad() const;
    Real toDeg() const;

    int compare(Real b) const;
    bool inBetween(Real min, Real max) const;
    bool operator < (Real b) const;
    bool operator > (Real b) const;
    bool operator <= (Real b) const;
    bool operator >= (Real b) const;
    bool operator == (Real b) const;
    bool operator != (Real b) const;

    Real lerp(Real start, Real end) const;
    Real lerp(Real inputStart, Real inputEnd, Real outputStart, Real outputEnd) const;
    Real clamp(Real min, Real max) const;

    Real floor() const;
    Real ceil() const;
    Real round() const;
    Real fractional() const;
    Real sqrt() const;
    Real invSqrt() const;

    static const Real one;
    static const Real zero;
    static const Real pi;
    static const Real rotationLeft;
    static const Real rotationRight;
};

//
// 2-dimensional Vector
//
namespace internal {
    template<class T> class Vector2Base;
    template<class T> class LineSeg;
    class Vector2XY;
    class Vector2XZ;
}
typedef internal::Vector2Base<internal::Vector2XY> XY;
typedef internal::Vector2Base<internal::Vector2XZ> XZ;
typedef internal::LineSeg<internal::Vector2XY> LineSegXY;
typedef internal::LineSeg<internal::Vector2XY> LineSegXZ;

namespace internal {
    template<class T> class Vector2Base : public T, public internal::CommonAssignArithmetic<Real>  {
        typedef Vector2Base<T> Me;
    public:
        Vector2Base(Real a, Real b) : T(a, b) {}

        Me operator + (Me b) const;
        Me operator - (Me b) const;
        Me operator * (Real f) const;
        Me operator / (Real f) const;

        Me operator - () const;

        Me rotate(Real angleInRad) const;
        Real cross(Me b) const;
        Real dot(Me b) const;
        Me orthogonal() const;

        Real lengthSqr() const;
        Real length() const;
        Me normalize() const;

        static const Me one, zero, right, left;
    };

    class Vector2XY {
    protected:
        inline Real& a() { return x; }
        inline Real& b() { return y; }
        inline Real a() const { return x; }
        inline Real b() const { return y; }
    public:
        Vector2XY(Real xx, Real yy): x(xx), y(yy) {}

        Real x, y;
        static const XY up, down;
    };

    class Vector2XZ {
    protected:
        inline Real& a() { return x; }
        inline Real& b() { return z; }
        inline Real a() const { return x; }
        inline Real b() const { return z; }
    public:
        Vector2XZ(Real xx, Real zz): x(xx), z(zz) {}

        Real x, z;
        static const XZ forward, backward;
    };

    template<class T> class LineSeg {
        typedef Vector2Base<T> Vec;
    public:
        Vec start, end;

        bool intersect(Vec& result) const;
        Real phase(Vec intersection) const;
    };

    extern template class Vector2Base<Vector2XY>;
    extern template class LineSeg<Vector2XY>;
    extern template class Vector2Base<Vector2XY>;
    extern template class LineSeg<Vector2XZ>;
}

#endif
