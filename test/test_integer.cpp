#include "fixtures.h"

class TestAlgebraInteger : public MathFixture { };

TEST_F(TestAlgebraInteger, lerpi) {
    EXPECT_EQ(0, lerpi(0, 0, 4, 0, 10));
    EXPECT_EQ(10, lerpi(4, 0, 4, 0, 10));
    EXPECT_EQ(5, lerpi(2, 0, 4, 0, 10));
    EXPECT_EQ(0, lerpi(2, 2, 4, 0, 10));
    EXPECT_EQ(5, lerpi(2, 2, 4, 5, 10));
    EXPECT_EQ(5, lerpi(0, -4, 4, 0, 10));
    EXPECT_EQ(-10, lerpi(2, 2, 4, -10, 10));
    EXPECT_EQ(5, lerpi(2, -4, 4, -10, 10));
    EXPECT_EQ(20, lerpi(8, 0, 4, 0, 10));
    EXPECT_EQ(-20, lerpi(-8, 0, 4, 0, 10));
}

TEST_F(TestAlgebraInteger, clampi) {
    EXPECT_EQ(5, clampi(0, 5, 10));
    EXPECT_EQ(0, clampi(0, 0, 10));
    EXPECT_EQ(0, clampi(0, -1, 10));
    EXPECT_EQ(10, clampi(0, 11, 10));
    EXPECT_EQ(-10, clampi(-10, -10, 0));
    EXPECT_EQ(-10, clampi(-10, -11, 0));
    EXPECT_EQ(-10, clampi(-10, -10, -5));
    EXPECT_EQ(-10, clampi(-10, -11, -5));
    EXPECT_EQ(-5, clampi(-10, 5, -5));
}
