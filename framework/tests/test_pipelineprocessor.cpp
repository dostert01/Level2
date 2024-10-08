#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>
#include "pipelineprocessor.h"
#include "../logger/logger.h"

using namespace second_take;

#define TEST_DATA_DIR "testdata"
#define PROCESS_CONFIG_TEST_FILE_01 TEST_DATA_DIR "/processConfig01.json"

namespace test_pipeline_processor {
    void configureLogger() {
        Logger& logger = second_take::Logger::getInstance();
        logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
    }
}

TEST(PipeLineProcessor, ReturnsEmptyOptionalIfConfigFileNotPresent) {
    test_pipeline_processor::configureLogger();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance("noFileHere");
    EXPECT_FALSE(processor.has_value());
}

TEST(PipeLineProcessor, CanLoadConfigFromJson) {
    test_pipeline_processor::configureLogger();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(PROCESS_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(processor.has_value());
    if(processor.has_value())
        EXPECT_TRUE((processor.value()->getCountOfPipelines() == 2));
}
