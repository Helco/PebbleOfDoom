#include "fixtures.h"

class TestIntersection : public MathFixture { };

TEST_F(TestIntersection, basic) {
    lineSeg_t seg1;
    seg1.start.xy = xy(real_zero, real_one);
    seg1.end.xy = xy(three, real_one);
    lineSeg_t seg2;
    seg2.start.xy = xy(real_one, real_zero);
    seg2.end.xy = xy(real_one, two);

    xy_t intersection;
    EXPECT_TRUE(xy_lineIntersect(seg1, seg2, &intersection));
    expectApproxEq(real_one, intersection.x);
    expectApproxEq(real_one, intersection.y);
    expectApproxEq(one_third, xy_linePhase(seg1, intersection));
    expectApproxEq(one_half, xy_linePhase(seg2, intersection));
}

TEST_F(TestIntersection, parallel) {
    lineSeg_t seg1;
    seg1.start.xy = xy(real_zero, real_zero);
    seg1.end.xy = xy(three, real_zero);
    lineSeg_t seg2;
    seg2.start.xy = xy(real_zero, real_zero);
    seg2.end.xy = xy(three, real_zero);

    xy_t intersection;
    EXPECT_FALSE(xy_lineIntersect(seg1, seg2, &intersection));
}

TEST_F(TestIntersection, out_of_segment) {
    lineSeg_t seg1;
    seg1.start.xz = xz(real_zero, real_one);
    seg1.end.xz = xz(three, real_one);
    lineSeg_t seg2;
    seg2.start.xz = xz(minus_one, real_zero);
    seg2.end.xz = xz(minus_one, two);

    xz_t intersection;
    EXPECT_TRUE(xz_lineIntersect(seg1, seg2, &intersection));
    expectApproxEq(minus_one, intersection.x);
    expectApproxEq(real_one, intersection.z);
    expectApproxEq(real_mul(one_third, minus_one), xz_linePhase(seg1, intersection));
    expectApproxEq(one_half, xz_linePhase(seg2, intersection));
}
