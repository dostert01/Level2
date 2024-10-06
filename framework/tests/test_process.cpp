#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>
#include "process.h"
#include "../logger/logger.h"

using namespace second_take;

#define TEST_DATA_DIR "testdata"
#define PROCESS_CONFIG_TEST_FILE_01 TEST_DATA_DIR "/processConfig01.json"

namespace test_process {
    void configureLogger() {
        Logger& logger = second_take::Logger::getInstance();
        logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
    }
}

TEST(Process, CanLoadConfigFromJson) {
    test_process::configureLogger();
    std::optional<std::unique_ptr<Process>> process = Process::getInstance(PROCESS_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(process.has_value());
    if(process.has_value())
        EXPECT_TRUE((process.value()->getCountOfPipelines() == 2));
}
