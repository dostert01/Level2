#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>
#include "pipeline.h"
#include "../logger/logger.h"

using namespace second_take;

#define TEST_DATA_DIR "testdata"
#define PIPELINE_CONFIG_TEST_FILE_01 TEST_DATA_DIR "/pipelineConfig01.json"
#define PIPELINE_CONFIG_TEST_FILE_02 TEST_DATA_DIR "/pipelineConfig02.json"
#define PIPELINE_CONFIG_TEST_FILE_03 TEST_DATA_DIR "/pipelineConfig03.json"

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

TEST(PipeLine, CanLoadTheHelloWordLib) {
    configureLogger();
    std::optional<std::unique_ptr<Pipeline>> pipeline1 = second_take::Pipeline::getInstance(PIPELINE_CONFIG_TEST_FILE_01);
    std::optional<PipelineStep*> pipelineStep = pipeline1.value()->getStepByName("test Step 02");
    EXPECT_TRUE(pipelineStep.has_value());
    if(pipelineStep.has_value()) {
        PipelineStep* currentStep = pipelineStep.value();
        EXPECT_TRUE(currentStep->isInitComplete());
    }
}

TEST(Pipeline, CanExecuteThePipeline) {
    configureLogger();
    std::optional<std::unique_ptr<Pipeline>> pipeline = second_take::Pipeline::getInstance(PIPELINE_CONFIG_TEST_FILE_01);
    testing::internal::CaptureStdout();
    pipeline.value()->execute();
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("Hello World");
    EXPECT_TRUE(std::regex_search(output, regex));
}


TEST(Pipeline, CanUseArgumentsFromInitDataInJson) {
    configureLogger();
    std::optional<std::unique_ptr<Pipeline>> pipeline = second_take::Pipeline::getInstance(PIPELINE_CONFIG_TEST_FILE_03);
    std::string testfileName = "./argumentsProcessor_output02.txt";
    std::filesystem::remove(testfileName);
    pipeline.value()->execute();
    std::ifstream testFile(testfileName.c_str());
    EXPECT_TRUE(testFile.is_open());
    std::string line;
    std::getline(testFile, line);
    EXPECT_EQ("first hello from step 2", line);
    std::getline(testFile, line);
    EXPECT_EQ("second hello from step 2", line);
    testFile.close();
    std::filesystem::remove(testfileName);
}

TEST(Pipeline, CanUseProcessData) {
    configureLogger();
    std::optional<std::unique_ptr<Pipeline>> pipeline = second_take::Pipeline::getInstance(PIPELINE_CONFIG_TEST_FILE_03);
    std::string testfileName = "./argumentsProcessor_output02.txt";
    std::filesystem::remove(testfileName);
    PipelineProcessingData processData;
    processData.addPayloadData("question", "text/plain", "What is the answer?");
    pipeline.value()->execute(processData);
    std::optional<ProcessingPayload*> payload;
    payload = processData.getPayload("question");
    EXPECT_TRUE(payload.has_value());
    payload = processData.getPayload("answer");
    EXPECT_TRUE(payload.has_value());
    EXPECT_EQ(payload.value()->payloadAsString().compare("the answer is 42"), 0);
    std::filesystem::remove(testfileName);
}
