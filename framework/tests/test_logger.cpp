#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

using namespace std;
using namespace event_forge;

TEST(Logger, AlwaysCreatesASingletonInstanceAsPointer) {
    Logger* logger1 = Logger::getInstanceAsPointer();
    Logger* logger2 = Logger::getInstanceAsPointer();
    EXPECT_EQ(logger1, logger2);
}

TEST(Logger, AlwaysCreatesASingletonInstanceAsReference) {
    Logger& logger1 = Logger::getInstance();
    Logger& logger2 = Logger::getInstance();
    EXPECT_EQ(logger1, logger2);
}

TEST(Logger, CanLogTraceToStdout) {
    Logger& logger = Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.trace("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[TRACE\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(Logger, CanLogDebugToStdout) {
    Logger& logger = Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.debug("Hello Debug");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[DEBUG\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Debug");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(Logger, CanLogInfoToStdout) {
    Logger& logger = Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.info("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[INFO\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(Logger, CanLogWarnToStdout) {
    Logger& logger = Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.warn("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[WARN\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(Logger, CanLogErrorToStdout) {
    Logger& logger = Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.error("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(Logger, CanLogFatalToStdout) {
    Logger& logger = Logger::getInstance();
    testing::internal::CaptureStdout();
    logger.fatal("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[FATAL\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(Logger, DoesNotLogInfoIfMaxLogLevelIsWarn) {
    Logger& logger = Logger::getInstance();
    logger.setMaxLogLevel(LogLevel::LOG_LEVEL_WARN);
    testing::internal::CaptureStdout();
    logger.info("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[INFO\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_FALSE(regex_search(output, regex));
}

TEST(Logger, LogsWarnIfMaxLogLevelIsWarn) {
    Logger& logger = Logger::getInstance();
    logger.setMaxLogLevel(LogLevel::LOG_LEVEL_WARN);
    testing::internal::CaptureStdout();
    logger.warn("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[WARN\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(Logger, LogsErrorIfMaxLogLevelIsWarn) {
    Logger& logger = Logger::getInstance();
    logger.setMaxLogLevel(LogLevel::LOG_LEVEL_WARN);
    testing::internal::CaptureStdout();
    logger.error("Hello Log");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[ERROR\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}

void setLogLevelEnv(const char* logLevelString) {
    if(setenv(MAX_LOG_LEVEL_ENV_VAR_NAME, logLevelString, 1) != 0) {
        cout << "WARNING: failed to set env " << MAX_LOG_LEVEL_ENV_VAR_NAME << endl;
    }
}

void unsetLogLevelEnv() {
    unsetenv(MAX_LOG_LEVEL_ENV_VAR_NAME);
}

TEST(Logger, environmentCanOverrideLogLevelSuppressing) {
    Logger& logger = Logger::getInstance();
    logger.setMaxLogLevel(LogLevel::LOG_LEVEL_DEBUG);
    setLogLevelEnv(LOG_PREFIX_ERROR);
    testing::internal::CaptureStdout();
    logger.debug("Hello Log");
    unsetLogLevelEnv();
    logger.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[DEBUG\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_FALSE(regex_search(output, regex));
}

TEST(Logger, environmentCanOverrideLogLevelNotSuppressing) {
    Logger& logger = Logger::getInstance();
    logger.setMaxLogLevel(LogLevel::LOG_LEVEL_FATAL);
    setLogLevelEnv(LOG_PREFIX_DEBUG);
    testing::internal::CaptureStdout();
    logger.debug("Hello Log");
    unsetLogLevelEnv();
    logger.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("^\\[DEBUG\\] \\[20([0-9]{2}-){2}[0-9]{2} ([0-9]{2}:){2}[0-9]{2}\\] Hello Log");
    EXPECT_TRUE(regex_search(output, regex));
}
