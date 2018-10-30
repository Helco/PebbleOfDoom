#include <gtest/gtest.h>

TEST(Basic, realBasic) {
    EXPECT_EQ(0, 0);
}

TEST(Basic, fails) {
    EXPECT_EQ(0, 1);
}
