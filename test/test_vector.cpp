#include "fixtures.h"

class TestAlgebraVector : public MathFixture { };

TEST_F(TestAlgebraVector, constants) {
    // To test macro evaluation
    xy_t xyone = xy_one;
    xy_t xyzero = xy_zero;
    xy_t xyright = xy_right;
    xy_t xyup = xy_up;
    xz_t xzone = xz_one;
    xz_t xzzero = xz_zero;
    xz_t xzright = xz_right;
    xz_t xzforward = xz_forward;
}

TEST_F(TestAlgebraVector, constructor) {
    xy_t v1 = xy(real_one, real_zero);
    EXPECT_EQ(real_one, v1.x);
    EXPECT_EQ(real_zero, v1.y);

    xz_t v2 = xz(real_2pi, real_pi);
    EXPECT_EQ(real_2pi, v2.x);
    EXPECT_EQ(real_pi, v2.z);
}

TEST_F(TestAlgebraVector, add) {
    xy_t v1 = xy_add(
        xy(real_one, real_pi),
        xy(minus_one, real_pi)
    );
    expectApproxEq(real_zero, v1.x);
    expectApproxEq(real_2pi, v1.y);
}

TEST_F(TestAlgebraVector, sub) {
    xy_t v1 = xy_sub(
        xy(real_one, real_pi),
        xy(minus_one, real_pi)
    );
    expectApproxEq(two, v1.x);
    expectApproxEq(real_zero, v1.y);
}

TEST_F(TestAlgebraVector, scale) {
    xy_t v1 = xy_scale(xy(real_one, one_third), two);
    expectApproxEq(v1.x, two);
    expectApproxEq(v1.y, two_thirds);

    xy_t v2 = xy_invScale(xy(two, real_2pi), two);
    expectApproxEq(v2.x, real_one);
    expectApproxEq(v2.y, real_pi);
}

TEST_F(TestAlgebraVector, cross) {
    real_t det1 = xy_cross(
        xy(real_one, minus_one),
        xy(two, real_one)
    );
    expectApproxEq(three, det1);
}

TEST_F(TestAlgebraVector, dot) {
    real_t dot1 = xy_dot(
        xy(real_one, one_third),
        xy(minus_one, three)
    );
    expectApproxEq(real_zero, dot1);
}

TEST_F(TestAlgebraVector, orthogonal) {
    xy_t v1 = xy(one_third, real_pi);
    xy_t v2 = xy_orthogonal(v1);
    xy_t v3 = xy_orthogonal(v2);

    expectApproxEq(real_zero, xy_dot(v1, v2));

    real_t l1 = xy_lengthSqr(v1);
    real_t l2 = xy_lengthSqr(v2);
    real_t l3 = xy_lengthSqr(v3);
    expectApproxEq(l1, l2);
    expectApproxEq(l2, l3);

    real_t v1_dot_v3 = real_abs(xy_dot(v1, v3));
    expectApproxEq(l1, v1_dot_v3);
}

TEST_F(TestAlgebraVector, lengthSqr) {
    xy_t v1 = xy(real_one, two);
    expectApproxEq(real_from_int(5), xy_lengthSqr(v1));

    xy_t v2 = xy(minus_one, real_one);
    expectApproxEq(two, xy_lengthSqr(v2));

    expectApproxEq(real_zero, xy_lengthSqr(xy_zero));
}

TEST_F(TestAlgebraVector, length) {
    xy_t v1 = xy(real_sqrt(real_from_int(7)), three);
    expectApproxEq(four, xy_length(v1));

    xy_t v2 = xy(real_zero, real_one);
    expectApproxEq(real_one, xy_length(v2));

    expectApproxEq(real_zero, xy_length(xy_zero));
}

TEST_F(TestAlgebraVector, normalize) {
    xy_t v1 = xy_normalize(xy(three, real_zero));
    expectApproxEq(real_one, v1.x);
    expectApproxEq(real_zero, v1.y);

    xy_t v2 = xy_normalize(xy(one_third, two_thirds));
    expectApproxEq(real_one, xy_length(v2));
}
