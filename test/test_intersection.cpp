#include "fixtures.h"

class TestIntersection : public MathFixture { };

TEST_F(TestIntersection, basic) {
    lineSeg_t seg1;
    seg1.start.xy = xy(real_zero, real_one);
    seg1.end.xy = xy(three, real_one);
    lineSeg_t seg2;
    seg2.start.xy = xy(real_one, real_zero);
    seg2.end.xy = xy(real_one, two);

    lineIntersection_t intersect;
    EXPECT_TRUE(xy_intersect(seg1, seg2, &intersect));
    expectApproxEq(real_one, intersect.xy.x);
    expectApproxEq(real_one, intersect.xy.y);
    expectApproxEq(one_third, intersect.phase1);
    expectApproxEq(one_half, intersect.phase2);
}

TEST_F(TestIntersection, parallel) {
    lineSeg_t seg1;
    seg1.start.xy = xy(real_zero, real_zero);
    seg1.end.xy = xy(three, real_zero);
    lineSeg_t seg2;
    seg2.start.xy = xy(real_zero, real_zero);
    seg2.end.xy = xy(three, real_zero);

    lineIntersection_t intersect;
    EXPECT_FALSE(xy_intersect(seg1, seg2, &intersect));
}

TEST_F(TestIntersection, out_of_segment) {
    lineSeg_t seg1;
    seg1.start.xy = xy(real_zero, real_one);
    seg1.end.xy = xy(three, real_one);
    lineSeg_t seg2;
    seg2.start.xy = xy(minus_one, real_zero);
    seg2.end.xy = xy(minus_one, two);

    lineIntersection_t intersect;
    EXPECT_TRUE(xy_intersect(seg1, seg2, &intersect));
    expectApproxEq(minus_one, intersect.xy.x);
    expectApproxEq(real_one, intersect.xy.y);
    expectApproxEq(real_mul(one_third, minus_one), intersect.phase1);
    expectApproxEq(one_half, intersect.phase2);
}
