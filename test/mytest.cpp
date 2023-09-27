#include <gtest/gtest.h>
int myfunctions() { return 0; }
int add(int a, int b) { return a + b; }
TEST(myfunctions, add) { GTEST_ASSERT_EQ(add(10, 22), 32); }

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
