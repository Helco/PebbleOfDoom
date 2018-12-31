#include "fixtures.h"

class TestLineSeg : public MathFixture { };

#define xyFromInt(x,y) (xy(real_from_int(x), real_from_int(y)))
#define lineSegFromInt(x1,y1,x2,y2) ((lineSeg_t) { \
    .start = { .xy = xyFromInt(x1, y1) }, \
    .end = { .xy = xyFromInt(x2, y2) }})

TEST_F(TestLineSeg, isOnRight_right) {
    /*
     * -------->
     */
    const lineSeg_t line = lineSegFromInt(0, 0, 10, 0);
    ASSERT_TRUE(xy_isOnRight(xyFromInt(5, -1), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(5, -10), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(5, 0), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(-50, -1), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(50, -1), line));

    ASSERT_FALSE(xy_isOnRight(xyFromInt(5, 1), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(5, 10), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(-50, 1), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(50, 1), line));
}

TEST_F(TestLineSeg, isOnRight_left) {
    /*
     * <---------
     */
    const lineSeg_t line = lineSegFromInt(10, 0, 0, 0);
    ASSERT_TRUE(xy_isOnRight(xyFromInt(5, 1), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(5, 10), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(5, 0), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(-50, 1), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(50, 1), line));

    ASSERT_FALSE(xy_isOnRight(xyFromInt(5, -1), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(5, -10), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(-50, -1), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(50, -1), line));
}

TEST_F(TestLineSeg, isOnRight_diagonal) {
    /*
     *     -->
     *   --/
     * --/
     */
    const lineSeg_t line = lineSegFromInt(0, 0, 10, 5);
    ASSERT_TRUE(xy_isOnRight(xyFromInt(0, 0), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(1, 0), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(10, 0), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(50, 0), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(10, 5), line));
    ASSERT_TRUE(xy_isOnRight(xyFromInt(8, 3), line));

    ASSERT_FALSE(xy_isOnRight(xyFromInt(0, 1), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(-1, 0), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(10, 7), line));
    ASSERT_FALSE(xy_isOnRight(xyFromInt(5, 5), line));
}
