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
