#include "fixtures.h"

class TestAlgebraReal : public MathFixture { };

TEST_F(TestAlgebraReal, real_constants) {
    EXPECT_EQ(real_one, real_from_int(1));
    EXPECT_EQ(real_zero, real_from_int(0));
    EXPECT_EQ(real_from_int(0), real_from_int(0));
    EXPECT_EQ(real_from_int(1), real_from_int(1));
    EXPECT_EQ(real_add(real_pi, real_pi), real_2pi);

    EXPECT_NE(real_one, real_zero);
    EXPECT_NE(real_pi, real_zero);
    EXPECT_NE(real_2pi, real_zero);
    EXPECT_NE(real_pi, real_one);
    EXPECT_NE(real_2pi, real_one);
    EXPECT_NE(real_pi, real_2pi);
}

TEST_F(TestAlgebraReal, real_from_float) {
    expectApproxEq(real_one, real_from_float(1.0f));
    expectApproxEq(real_zero, real_from_float(0.0f));
    expectApproxEq(minus_one, real_from_float(-1.0f));
    expectApproxEq(one_third, real_from_float(0.3333333f));
    expectApproxEq(two_thirds, real_from_float(0.6666666f));
    expectApproxEq(real_pi, real_from_float(3.141592653f));
}

TEST_F(TestAlgebraReal, real_to_float) {
    EXPECT_FLOAT_EQ(1.0f, real_to_float(real_one));
    EXPECT_FLOAT_EQ(0.0f, real_to_float(real_zero));
    EXPECT_FLOAT_EQ(-1.0f, real_to_float(minus_one));
    EXPECT_FLOAT_EQ(0.3333333f, real_to_float(one_third));
    EXPECT_FLOAT_EQ(0.6666666f, real_to_float(two_thirds));
    EXPECT_FLOAT_EQ(3.141592653f, real_to_float(real_pi));
}

TEST_F(TestAlgebraReal, real_compare) {
    EXPECT_EQ(0, real_compare(real_zero, real_zero));
    EXPECT_EQ(0, real_compare(real_one, real_one));

    EXPECT_LT(0, real_compare(real_one, real_zero));
    EXPECT_GT(0, real_compare(real_zero, real_one));

    EXPECT_LT(0, real_compare(real_zero, minus_one));
    EXPECT_LT(0, real_compare(real_one, minus_one));
    EXPECT_GT(0, real_compare(minus_one, real_zero));
    EXPECT_GT(0, real_compare(minus_one, real_one));

    EXPECT_LT(0, real_compare(positive, negative));
    EXPECT_GT(0, real_compare(negative, positive));
}

TEST_F(TestAlgebraReal, real_inBetween) {
    EXPECT_TRUE(real_inBetween(one_half, real_zero, real_one));
    EXPECT_TRUE(real_inBetween(real_zero, real_zero, real_one));
    EXPECT_TRUE(real_inBetween(real_one, real_zero, real_one));
    EXPECT_FALSE(real_inBetween(minus_one, real_zero, real_one));
    EXPECT_FALSE(real_inBetween(negative, real_zero, real_one));
    EXPECT_FALSE(real_inBetween(four, real_zero, real_one));
    EXPECT_FALSE(real_inBetween(four, minus_one, three));
    EXPECT_TRUE(real_inBetween(real_zero, negative, positive));

    EXPECT_FALSE(real_inBetween(real_zero, positive, negative));
}

TEST_F(TestAlgebraReal, real_minmax) {
    EXPECT_EQ(negative, real_min(negative, positive));
    EXPECT_EQ(negative, real_min(positive, negative));
    EXPECT_EQ(positive, real_max(negative, positive));
    EXPECT_EQ(positive, real_max(positive, negative));

    EXPECT_EQ(negative, real_min(negative, negative));
    EXPECT_EQ(negative, real_max(negative, negative));
    EXPECT_EQ(positive, real_min(positive, positive));
    EXPECT_EQ(positive, real_max(positive, positive));

    EXPECT_EQ(real_zero, real_max(negative, real_zero));
    EXPECT_EQ(real_zero, real_min(positive, real_zero));
}

TEST_F(TestAlgebraReal, real_abs) {
    EXPECT_EQ(real_one, real_abs(minus_one));
    EXPECT_EQ(real_one, real_abs(real_one));
    EXPECT_EQ(real_zero, real_abs(real_zero));
    EXPECT_EQ(positive, real_abs(positive));

    EXPECT_EQ(real_mul(minus_one, negative), real_abs(negative));
}

TEST_F(TestAlgebraReal, real_signInt) {
    EXPECT_EQ(1, real_signInt(real_one));
    EXPECT_EQ(1, real_signInt(real_zero));
    EXPECT_EQ(1, real_signInt(positive));
    EXPECT_EQ(-1, real_signInt(negative));
    EXPECT_EQ(-1, real_signInt(minus_one));
}

TEST_F(TestAlgebraReal, real_add) {
    EXPECT_EQ(real_one, real_add(real_zero, real_one));
    EXPECT_EQ(real_zero, real_add(minus_one, real_one));
    EXPECT_EQ(real_zero, real_add(real_one, minus_one));
    EXPECT_EQ(real_zero, real_add(real_zero, real_zero));
    EXPECT_EQ(real_from_int(2), real_add(real_one, real_one));
    EXPECT_EQ(real_one, real_add(one_third, two_thirds));
    EXPECT_EQ(real_one, real_add(two_thirds, one_third));
}

TEST_F(TestAlgebraReal, real_sub) {
    EXPECT_EQ(minus_one, real_sub(real_zero, real_one));
    EXPECT_EQ(real_from_int(-2), real_sub(minus_one, real_one));
    EXPECT_EQ(real_from_int(2), real_sub(real_one, minus_one));
    EXPECT_EQ(real_zero, real_sub(real_zero, real_zero));
    EXPECT_EQ(real_zero, real_sub(real_one, real_one));
    EXPECT_EQ(one_third, real_sub(two_thirds, one_third));
}

TEST_F(TestAlgebraReal, real_mul) {
    EXPECT_EQ(real_from_int(12), real_mul(three, real_from_int(4)));
    EXPECT_EQ(real_from_int(12), real_mul(real_from_int(12), real_one));
    EXPECT_EQ(real_zero, real_mul(real_zero, real_one));
    EXPECT_EQ(real_zero, real_mul(real_one, real_zero));
    EXPECT_EQ(real_zero, real_mul(real_zero, real_zero));
    EXPECT_EQ(real_one, real_mul(real_one, real_one));
    EXPECT_EQ(real_one, real_mul(minus_one, minus_one));
    EXPECT_EQ(minus_one, real_mul(real_one, minus_one));
    EXPECT_EQ(minus_one, real_mul(minus_one, real_one));
    EXPECT_EQ(real_one, real_mul(three, one_third));
}

TEST_F(TestAlgebraReal, real_div) {
    EXPECT_EQ(real_from_int(4), real_div(real_from_int(12), three));
    EXPECT_EQ(real_zero, real_div(real_zero, real_one));
    EXPECT_EQ(real_zero, real_div(real_zero, minus_one));
    EXPECT_EQ(real_zero, real_div(real_zero, two_thirds));
    EXPECT_EQ(real_one, real_div(real_one, real_one));
    EXPECT_EQ(real_one, real_div(one_third, one_third));
    EXPECT_EQ(one_third, real_div(real_one, three));
}

TEST_F(TestAlgebraReal, real_reciprocal) {
    EXPECT_EQ(one_third, real_reciprocal(three));
    EXPECT_EQ(three, real_reciprocal(one_third));
    EXPECT_EQ(real_one, real_reciprocal(real_one));
    EXPECT_EQ(minus_one, real_reciprocal(minus_one));
}

TEST_F(TestAlgebraReal, real_sincostan) {
    expectApproxEq(real_zero, real_sin(real_zero));
    expectApproxEq(real_one, real_sin(halfpi));
    expectApproxEq(real_zero, real_sin(real_pi));
    expectApproxEq(minus_one, real_sin(real_add(real_pi, halfpi)));
    expectApproxEq(real_zero, real_sin(real_2pi));

    expectApproxEq(real_one, real_cos(real_zero));
    expectApproxEq(real_zero, real_cos(halfpi));
    expectApproxEq(minus_one, real_cos(real_pi));
    expectApproxEq(real_zero, real_cos(real_add(real_pi, halfpi)));
    expectApproxEq(real_one, real_cos(real_2pi));

    expectApproxEq(real_zero, real_tan(real_zero));
    EXPECT_LT(0, real_compare(real_tan(real_one), real_one));
    EXPECT_GT(0, real_compare(real_tan(two), minus_one));
    expectApproxEq(real_zero, real_tan(real_pi));
    EXPECT_LT(0, real_compare(real_tan(four), real_one));

    expectApproxEq(real_tan(real_zero), real_div(real_sin(real_zero), real_cos(real_zero)));
    expectApproxEq(real_tan(real_one), real_div(real_sin(real_one), real_cos(real_one)));
    expectApproxEq(real_tan(one_third), real_div(real_sin(one_third), real_cos(one_third)));
    expectApproxEq(real_tan(two_thirds), real_div(real_sin(two_thirds), real_cos(two_thirds)));
}

TEST_F(TestAlgebraReal, real_degToRad) {
    expectApproxEq(real_zero, real_degToRad(real_from_int(0)));
    expectApproxEq(halfpi, real_degToRad(real_from_int(90)));
    expectApproxEq(real_pi, real_degToRad(real_from_int(180)));
    expectApproxEq(real_add(real_pi, halfpi), real_degToRad(real_from_int(270)));
    expectApproxEq(real_2pi, real_degToRad(real_from_int(360)));
}

TEST_F(TestAlgebraReal, real_radToDeg) {
    expectApproxEq(real_from_int(0), real_radToDeg(real_zero));
    expectApproxEq(real_from_int(90), real_radToDeg(halfpi));
    expectApproxEq(real_from_int(180), real_radToDeg(real_pi));
    expectApproxEq(real_from_int(270), real_radToDeg(real_add(real_pi, halfpi)));
    expectApproxEq(real_from_int(360), real_radToDeg(real_2pi));
}

TEST_F(TestAlgebraReal, real_floor) {
    expectApproxEq(real_zero, real_floor(one_half));
    expectApproxEq(real_zero, real_floor(one_third));
    expectApproxEq(real_zero, real_floor(two_thirds));
    expectApproxEq(real_one, real_floor(real_one));
    expectApproxEq(real_zero, real_floor(real_zero));
    expectApproxEq(minus_one, real_floor(minus_one));
    expectApproxEq(minus_one, real_floor(negative));
}

TEST_F(TestAlgebraReal, real_ceil) {
    expectApproxEq(real_one, real_ceil(one_half));
    expectApproxEq(real_one, real_ceil(one_third));
    expectApproxEq(real_one, real_ceil(two_thirds));
    expectApproxEq(real_one, real_ceil(real_one));
    expectApproxEq(real_zero, real_ceil(real_zero));
    expectApproxEq(minus_one, real_ceil(minus_one));
    expectApproxEq(real_zero, real_ceil(negative));
}

TEST_F(TestAlgebraReal, real_round) {
    expectApproxEq(real_one, real_round(one_half));
    expectApproxEq(real_zero, real_round(one_third));
    expectApproxEq(real_one, real_round(two_thirds));
    expectApproxEq(real_zero, real_round(negative));
    expectApproxEq(real_one, real_round(real_one));
    expectApproxEq(minus_one, real_round(minus_one));
    expectApproxEq(real_zero, real_round(real_zero));
}

TEST_F(TestAlgebraReal, real_fractional) {
    expectApproxEq(one_half, real_fractional(one_half));
    expectApproxEq(one_third, real_fractional(one_third));
    expectApproxEq(two_thirds, real_fractional(two_thirds));
    expectApproxEq(real_zero, real_fractional(real_zero));
    expectApproxEq(real_zero, real_fractional(real_one));
    expectApproxEq(real_zero, real_fractional(minus_one));

    real_t fractPi = real_fractional(real_pi);
    expectApproxEq(fractPi, real_sub(real_pi, real_floor(real_pi)));
}

TEST_F(TestAlgebraReal, real_sqrt) {
    expectApproxEq(real_one, real_sqrt(real_one));
    expectApproxEq(three, real_sqrt(real_from_int(9)));
    expectApproxEq(two, real_sqrt(four));

    real_t sqrtTwo = real_sqrt(two);
    expectApproxEq(two, real_mul(sqrtTwo, sqrtTwo));
}

TEST_F(TestAlgebraReal, real_invSqrt) {
    expectApproxEq(real_one, real_invSqrt(real_one));
    expectApproxEq(one_third, real_invSqrt(real_from_int(9)));

    real_t invSqrtTwo = real_invSqrt(two);
    expectApproxEq(real_sqrt(two), real_reciprocal(invSqrtTwo));
}
