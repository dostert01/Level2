#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>
#include "pipelineprocessor.h"
#include "../logger/logger.h"

using namespace second_take;

#ifndef LOGGER
#define LOGGER second_take::Logger::getInstance()
#endif

#define PROCESS_CONFIG_TEST_FILE_01 "/processConfig01.json"

namespace test_pipeline_processor {
    std::string workingDir;
    std::string testFilesDir;

    void configureLogger() {
        LOGGER.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_TRACE);
        LOGGER.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
    }

    void configureTestVariables() {
        workingDir = std::filesystem::current_path();
        LOGGER.info("Running test in directory: " + workingDir);
        if(std::getenv("TEST_FILES_DIR") != NULL) {
            testFilesDir = std::getenv("TEST_FILES_DIR");
            LOGGER.info("Testfiles expected to be present in directory: " + testFilesDir);
        } else {
            LOGGER.error("Environment variable TEST_FILES_DIR must be set to where the tests expect the testfiles to live!");
        }  
    }
}

void configureTest() {
    test_pipeline_processor::configureLogger();
    test_pipeline_processor::configureTestVariables();
}

TEST(PipeLineProcessor, ReturnsEmptyOptionalIfConfigFileNotPresent) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance("noFileHere");
    EXPECT_FALSE(processor.has_value());
}

TEST(PipeLineProcessor, CanLoadConfigFromJson) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(processor.has_value());
    if(processor.has_value())
        EXPECT_TRUE((processor.value()->getCountOfPipelines() == 2));
}

TEST(PipeLineProcessor, ProcessHasAName) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    EXPECT_EQ("my first testProcess", processor.value()->getProcessName());
}

TEST(PipeLineProcessor, CanGetPipelineByName) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    PipeLineProcessor* prc = processor.value().get();

    //"my first testPipeline"

    EXPECT_TRUE(false);
}

TEST(PipeLineProcessor, PipelineHasSelectionPatterns) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(false);
}
