#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>
#include "pipeline.h"
#include "../logger/logger.h"
#include "binaryDataProcessor.h"

using namespace event_forge;

#define PIPELINE_CONFIG_TEST_FILE_01 "/pipelineConfig01.json"
#define PIPELINE_CONFIG_TEST_FILE_02 "/pipelineConfig02.json"
#define PIPELINE_CONFIG_TEST_FILE_03 "/pipelineConfig03.json"
#define PIPELINE_CONFIG_TEST_FILE_04 "/pipelineConfig04.json"
#define PIPELINE_CONFIG_TEST_FILE_05 "/pipelineConfig05.json"

namespace test_pipeline {
    std::string workingDir;
    std::string testFilesDir;

    void configureLogger() {
        Logger& logger = Logger::getInstance();
        logger.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
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
    test_pipeline::configureLogger();
    test_pipeline::configureTestVariables();
}

TEST(PipeLine, CanCreateNewInstances) {
    std::shared_ptr<Pipeline> pipeline1 = Pipeline::getInstance();
    std::shared_ptr<Pipeline> pipeline2 = Pipeline::getInstance();
    EXPECT_FALSE(pipeline1.get() == pipeline2.get());
}

TEST(PipeLine, CanLoadConfigFromJson) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline1 = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(pipeline1.has_value());
    if(pipeline1.has_value())
        EXPECT_TRUE((pipeline1.value()->getCountOfPipelineSteps() == 2));
}

TEST(PipeLine, ReturnsEmptyIfLoadingFails) {
    test_pipeline::configureLogger();
    std::optional<std::shared_ptr<Pipeline>> pipeline1 = Pipeline::getInstance("/this/is/and/invalid/path");
    EXPECT_FALSE(pipeline1.has_value());
}


TEST(PipeLine, PipelineHasName) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline1 = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(pipeline1.value()->getPipelineName() == "my first testPipeline");
}

TEST(PipeLine, CanLoadTheHelloWordLib) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline1 = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01);
    std::optional<std::shared_ptr<PipelineStep>> pipelineStep = pipeline1.value()->getStepByName("test Step 02");
    EXPECT_TRUE(pipelineStep.has_value());
    if(pipelineStep.has_value()) {
        PipelineStep* currentStep = pipelineStep.value().get();
        EXPECT_TRUE(currentStep->isInitComplete());
    }
}

TEST(Pipeline, CanExecuteThePipeline) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01);
    testing::internal::CaptureStdout();
    pipeline.value()->execute();
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << "output: " << output << std::endl;
    std::regex regex("Hello World");
    EXPECT_TRUE(std::regex_search(output, regex));
}


TEST(Pipeline, CanUseArgumentsFromInitDataInJson) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03);
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
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03);
    std::string testfileName = "./argumentsProcessor_output02.txt";
    std::filesystem::remove(testfileName);
    PipelineProcessingData processData;
    processData.addPayloadData("question", "text/plain", "What is the answer?");
    pipeline.value()->execute(processData);
    std::optional<std::shared_ptr<ProcessingPayload>> payload;
    payload = processData.getPayload("question");
    EXPECT_TRUE(payload.has_value());
    payload = processData.getPayload("answer");
    EXPECT_TRUE(payload.has_value());
    EXPECT_EQ(payload.value()->payloadAsString().compare("the answer is 42"), 0);
    std::filesystem::remove(testfileName);
}

TEST(Pipeline, CanUseBinaryProcessData) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    PipelineProcessingData processData;
    pipeline.value()->execute(processData);
    EXPECT_EQ(1, processData.getCountOfPayloads());
    std::optional<std::shared_ptr<ProcessingPayload>> payload;
    payload = processData.getPayload("myBinaryPayloadData");
    EXPECT_TRUE(payload.has_value());
    EXPECT_EQ("first hello from arguments",
        ((SpecificBinaryProcessingData*)(payload.value()->payloadAsBinaryData().get()))->firstArgument);    
    EXPECT_EQ("second hello from arguments",
        ((SpecificBinaryProcessingData*)(payload.value()->payloadAsBinaryData().get()))->secondArgument);
}

TEST(Pipeline, PipelineCanHaveSelectorPatterns) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key02", "value02");
    EXPECT_EQ(2, pipeline.value()->getCountOfMatchingPatterns());
}

TEST(Pipeline, SelectorPatternCanBeOverwritten) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key02", "value02");
    EXPECT_EQ("value02", pipeline.value()->getMatchingPattern("key02").value());
    pipeline.value()->addMatchingPattern("key02", "value03");
    EXPECT_EQ("value03", pipeline.value()->getMatchingPattern("key02").value());
    EXPECT_EQ(2, pipeline.value()->getCountOfMatchingPatterns());
}

TEST(Pipeline, SelectorPatternCanBeRemoved) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key02", "value02");
    pipeline.value()->removeMatchingPattern("key01");
    EXPECT_FALSE(pipeline.value()->getMatchingPattern("key01").has_value());
    EXPECT_EQ(1, pipeline.value()->getCountOfMatchingPatterns());
}

TEST(Pipeline, RemovingNonExistingSelectorHasNoEffect) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key02", "value02");
    pipeline.value()->removeMatchingPattern("key03");
    EXPECT_EQ(2, pipeline.value()->getCountOfMatchingPatterns());
}

TEST(Pipeline, ProcessesDataOnlyIfPayloadPatternsMatchThePipelinePatterns) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03);
    std::string testfileName = "./argumentsProcessor_output02.txt";
    std::filesystem::remove(testfileName);
    PipelineProcessingData processData;
    processData.addPayloadData("question", "text/plain", "What is the answer?");
    processData.addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->execute(processData);
    std::optional<std::shared_ptr<ProcessingPayload>> payload;
    payload = processData.getPayload("answer");
    EXPECT_TRUE(payload.has_value());
    EXPECT_EQ(payload.value()->payloadAsString().compare("the answer is 42"), 0);
    std::filesystem::remove(testfileName);
}

TEST(Pipeline, DoesNotProcessDataIfMatchingPatternsDiffer) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03);
    std::string testfileName = "./argumentsProcessor_output02.txt";
    std::filesystem::remove(testfileName);
    PipelineProcessingData processData;
    processData.addPayloadData("question", "text/plain", "What is the answer?");
    processData.addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key01", "value02");
    pipeline.value()->execute(processData);
    std::optional<std::shared_ptr<ProcessingPayload>> payload;
    payload = processData.getPayload("answer");
    EXPECT_FALSE(payload.has_value());
    std::filesystem::remove(testfileName);
}

TEST(Pipeline, PipelineCanLogErrors) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_05);
    PipelineProcessingData processData;
    pipeline.value()->execute(processData);
    EXPECT_TRUE(processData.hasError());
}

TEST(Pipeline, CanGetAllErrors) {
    configureTest();
    std::optional<std::shared_ptr<Pipeline>> pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_05);
    PipelineProcessingData processData;
    pipeline.value()->execute(processData);
    std::vector<ProcessingError> allErrors = processData.getAllErrors();
    EXPECT_EQ(2, allErrors.size());
    EXPECT_EQ("first error", allErrors.at(0).getErrorCode());
    EXPECT_EQ("this is a test error", allErrors.at(0).getErrorMessage());
    EXPECT_EQ("second error", allErrors.at(1).getErrorCode());
    EXPECT_EQ("this is yet another test error", allErrors.at(1).getErrorMessage());
}
