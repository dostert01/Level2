#include <gtest/gtest.h>
#include <iostream>

TEST(firstTest, Display) {
    testing::internal::CaptureStdout();
    std::cout << "Hello from a Test" << std::endl;
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Hello from a Test\n");
}

// Example test case
TEST(SampleTest, AssertionTrue) {
    EXPECT_TRUE(true);
}
