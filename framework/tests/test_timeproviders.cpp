#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <string>

#include "timeproviders.h"

using namespace event_forge;

TEST(TimeProvider, DefaultTimeProviderIsInstanceOfTimeProvider) {
    DefaultTimeProvider provider;
    EXPECT_TRUE(dynamic_cast<TimeProvider*>(&provider) != NULL);
}

TEST(TimeProvider, DefaultTimeProviderReturnsATimeStamp) {
    DefaultTimeProvider provider;
    std::string timeStamp = provider.getTimeStampOfNow();
    std::cout << "timeStamp: " << timeStamp << std::endl;
    std::regex regex("^20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}");
    EXPECT_TRUE(std::regex_search(timeStamp, regex));
}
