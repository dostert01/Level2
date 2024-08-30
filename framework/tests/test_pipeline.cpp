#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <iostream>
#include "pipeline.h"
#include "../logger/logger.h"

using namespace second_take;

#define TEST_DATA_DIR "testdata"
#define PIPELINE_CONFIG_TEST_FILE_01 TEST_DATA_DIR "/pipelineConfig01.json"
#define PIPELINE_CONFIG_TEST_FILE_02 TEST_DATA_DIR "/pipelineConfig02.json"

void configureLogger() {
    Logger& logger = second_take::Logger::getInstance();
    logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_TRACE);
    logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
}

TEST(PipeLine, CanCreateNewInstances) {
    std::unique_ptr<Pipeline> pipeline1 = second_take::Pipeline::getInstance();
    std::unique_ptr<Pipeline> pipeline2 = second_take::Pipeline::getInstance();
    EXPECT_FALSE(pipeline1.get() == pipeline2.get());
}

TEST(PipeLine, CanLoadConfigFromJson) {
    std::optional<std::unique_ptr<Pipeline>> pipeline1 = second_take::Pipeline::getInstance(PIPELINE_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(pipeline1.has_value());
    if(pipeline1.has_value())
        EXPECT_TRUE((pipeline1.value()->getCountOfPipelineSteps() == 2));
}

TEST(PipeLine, ReturnsEmptyIfLoadingFails) {
    configureLogger();
    std::optional<std::unique_ptr<Pipeline>> pipeline1 = second_take::Pipeline::getInstance("/this/is/and/invalid/path");
    EXPECT_FALSE(pipeline1.has_value());
}


TEST(PipeLine, PipelineHasName) {
    configureLogger();
    std::optional<std::unique_ptr<Pipeline>> pipeline1 = second_take::Pipeline::getInstance(PIPELINE_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(pipeline1.value()->getPipelineName() == "my first testPipeline");
}
