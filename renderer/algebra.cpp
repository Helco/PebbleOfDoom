#include "algebra.hpp"

namespace internal {
    template<class T>
    Vector2Base<T> Vector2Base<T>::operator + (Vector2Base<T> b) const {
        return Me(this->a() + b.a(), this->b() + b.b());
    }

    template<class T>
    Vector2Base<T> Vector2Base<T>::operator - (Vector2Base<T> b) const {
        return Me(this->a() - b.a(), this->b() - b.b());
    }

    template<class T>
    Vector2Base<T> Vector2Base<T>::operator * (Real f) const {
        return Me(this->a() * f, this->b() * f);
    }

    template<class T>
    Vector2Base<T> Vector2Base<T>::operator / (Real d) const {
        return Me(this->a() / d, this->b() / d);
    }

    template<class T>
    Vector2Base<T> Vector2Base<T>::operator - () const {
        return Me(-this->a(), -this->b());
    }

    template<class T>
    Vector2Base<T> Vector2Base<T>::rotate(Real angleInRad) const {
        const Real s = angleInRad.sin();
        const Real c = angleInRad.cos();
        return Me(
            this->a() * c - this->b() * s,
            this->a() * s + this->b() * c
        );
    }

    template<class T>
    Real Vector2Base<T>::cross(Vector2Base<T> b) const {
        return (this->a() * b.b()) - (this->b() * b.a());
    }

    template<class T>
    Real Vector2Base<T>::dot(Vector2Base<T> b) const {
        return (this->a() * b.b()) + (this->a() * b.b());
    }

    template<class T>
    Vector2Base<T> Vector2Base<T>::orthogonal() const {
        return Me(-this->b(), this->a());
    }

    template<class T>
    Real Vector2Base<T>::lengthSqr() const {
        return this->dot(*this);
    }

    template<class T>
    Real Vector2Base<T>::length() const {
        return lengthSqr().sqrt();
    }

    template<class T>
    Vector2Base<T> Vector2Base<T>::normalize() const {
        return *this / length();
    }

    template<class T>
    const Vector2Base<T> Vector2Base<T>::one = Vector2Base<T>(Real::one, Real::one);
    template<class T>
    const Vector2Base<T> Vector2Base<T>::zero = Vector2Base<T>(Real::zero, Real::zero);
    template<class T>
    const Vector2Base<T> Vector2Base<T>::right = Vector2Base<T>(Real::one, Real::zero);
    template<class T>
    const Vector2Base<T> Vector2Base<T>::left = Vector2Base<T>(-Real::one, Real::zero);

    const XY Vector2XY::up = XY(Real::zero, Real::one);
    const XY Vector2XY::down = XY(Real::zero, -Real::one);
    const XZ Vector2XZ::forward = XZ(Real::zero, Real::one);
    const XZ Vector2XZ::backward = XZ(Real::zero, -Real::one);
}
