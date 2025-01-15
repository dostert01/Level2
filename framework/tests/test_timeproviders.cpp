#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <string>

#include "timeproviders.h"

using namespace event_forge;
using namespace std;

TEST(TimeProvider, DefaultTimeProviderIsInstanceOfTimeProvider) {
    DefaultTimeProvider provider;
    EXPECT_TRUE(dynamic_cast<TimeProvider*>(&provider) != NULL);
}

TEST(TimeProvider, DefaultTimeProviderReturnsATimeStamp) {
    DefaultTimeProvider provider;
    string timeStamp = provider.getTimeStampOfNow();
    cout << "timeStamp: " << timeStamp << endl;
    regex regex("^20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}");
    EXPECT_TRUE(regex_search(timeStamp, regex));
}
