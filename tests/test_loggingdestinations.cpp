#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include "logger.h"

TEST(LoggingDestinations, DefaultIsStdOut) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.error("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(LoggingDestinations, CanRemoveAllLoggers) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    EXPECT_EQ(1, logger.getCountOfLoggingDestinations());
    logger.removeAllDestinations();
    EXPECT_EQ(0, logger.getCountOfLoggingDestinations());
}
