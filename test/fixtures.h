#include <gtest/gtest.h>

// Ensure renderer functions are linked correctly
extern "C" {
#include "../renderer/algebra.h"
}

class MathFixture : public testing::Test {
protected:
    const real_t minus_one = real_from_int(-1);
    const real_t two = real_from_int(2);
    const real_t positive = real_from_float(1 / 42.0f);
    const real_t negative = real_from_float(-1 / 56.0f);
    const real_t one_half = real_from_float(0.5f);
    const real_t one_third = real_from_float(1 / 3.0f);
    const real_t two_thirds = real_from_float(2 / 3.0f);
    const real_t three = real_from_int(3);
    const real_t four = real_from_int(4);
    const real_t five = real_from_int(5);
    const real_t ten = real_from_int(10);
    const real_t halfpi = real_div(real_pi, real_from_int(2));

#if defined REAL_USE_FLOAT
    void expectApproxEq(real_t expected, real_t actual)
    {
        real_t precision = 0.0001f;
        EXPECT_LE(expected - precision, actual);
        EXPECT_LE(actual, expected + precision);
    }
#else
#error "No real_t implementation specified"
#endif
};
