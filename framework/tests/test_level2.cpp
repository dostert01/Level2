#include <gtest/gtest.h>
#include <iostream>

using namespace std;

TEST(firstTest, Display) {
    testing::internal::CaptureStdout();
    cout << "Hello from a Test" << endl;
    string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Hello from a Test\n");
}

// Example test case
TEST(SampleTest, AssertionTrue) {
    EXPECT_TRUE(true);
}
