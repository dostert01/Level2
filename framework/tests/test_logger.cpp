#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

TEST(Logger, AlwaysCreatesASingletonInstanceAsPointer) {
    second_take::Logger* logger1 = second_take::Logger::getInstanceAsPointer();
    second_take::Logger* logger2 = second_take::Logger::getInstanceAsPointer();
    EXPECT_EQ(logger1, logger2);
}

TEST(Logger, AlwaysCreatesASingletonInstanceAsReference) {
    second_take::Logger& logger1 = second_take::Logger::getInstance();
    second_take::Logger& logger2 = second_take::Logger::getInstance();
    EXPECT_EQ(logger1, logger2);
}

TEST(Logger, CanLogTraceToStdout) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.trace("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[TRACE\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(Logger, CanLogDebugToStdout) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.debug("Hello Debug");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[DEBUG\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Debug");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(Logger, CanLogInfoToStdout) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.info("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[INFO\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(Logger, CanLogWarnToStdout) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.warn("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[WARN\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(Logger, CanLogErrorToStdout) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.error("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(Logger, CanLogFatalToStdout) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.fatal("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[FATAL\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(Logger, DoesNotLogInfoIfMaxLogLevelIsWarn) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_WARN);
    testing::internal::CaptureStdout();
    logger.info("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[INFO\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_FALSE(std::regex_search(output, regex));
}

TEST(Logger, LogsWarnIfMaxLogLevelIsWarn) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_WARN);
    testing::internal::CaptureStdout();
    logger.warn("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[WARN\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

TEST(Logger, LogsErrorIfMaxLogLevelIsWarn) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_WARN);
    testing::internal::CaptureStdout();
    logger.error("Hello Log");
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}

void setLogLevelEnv(const char* logLevelString) {
    if(setenv(MAX_LOG_LEVEL_ENV_VAR_NAME, logLevelString, 1) != 0) {
        std::cout << "WARNING: failed to set env " << MAX_LOG_LEVEL_ENV_VAR_NAME << std::endl;
    }
}

void unsetLogLevelEnv() {
    unsetenv(MAX_LOG_LEVEL_ENV_VAR_NAME);
}

TEST(Logger, environmentCanOverrideLogLevelSuppressing) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_DEBUG);
    setLogLevelEnv(LOG_PREFIX_ERROR);
    testing::internal::CaptureStdout();
    logger.debug("Hello Log");
    unsetLogLevelEnv();
    logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_TRACE);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[DEBUG\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_FALSE(std::regex_search(output, regex));
}

TEST(Logger, environmentCanOverrideLogLevelNotSuppressing) {
    second_take::Logger& logger = second_take::Logger::getInstance();
    logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_FATAL);
    setLogLevelEnv(LOG_PREFIX_DEBUG);
    testing::internal::CaptureStdout();
    logger.debug("Hello Log");
    unsetLogLevelEnv();
    logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_TRACE);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("^\\[DEBUG\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(std::regex_search(output, regex));
}
