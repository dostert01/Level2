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
using namespace std;

#define PIPELINE_CONFIG_TEST_FILE_01 "/pipelineConfig01.json"
#define PIPELINE_CONFIG_TEST_FILE_02 "/pipelineConfig02.json"
#define PIPELINE_CONFIG_TEST_FILE_03 "/pipelineConfig03.json"
#define PIPELINE_CONFIG_TEST_FILE_04 "/pipelineConfig04.json"
#define PIPELINE_CONFIG_TEST_FILE_05 "/pipelineConfig05.json"

namespace test_pipeline {
    string workingDir;
    string testFilesDir;

    void configureLogger() {
        Logger& logger = Logger::getInstance();
        logger.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
    }

    void configureTestVariables() {
        workingDir = filesystem::current_path();
        LOGGER.info("Running test in directory: " + workingDir);
        if(getenv("TEST_FILES_DIR") != NULL) {
            testFilesDir = getenv("TEST_FILES_DIR");
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
    auto pipeline1 = Pipeline::getInstance();
    auto pipeline2 = Pipeline::getInstance();
    EXPECT_FALSE(pipeline1.get() == pipeline2.get());
}

TEST(PipeLine, CanLoadConfigFromJson) {
    configureTest();
    auto pipeline1 = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(pipeline1.has_value());
    if(pipeline1.has_value())
        EXPECT_TRUE((pipeline1.value()->getCountOfPipelineSteps() == 2));
}

TEST(PipeLine, ReturnsEmptyIfLoadingFails) {
    test_pipeline::configureLogger();
    auto pipeline1 = Pipeline::getInstance("/this/is/and/invalid/path");
    EXPECT_FALSE(pipeline1.has_value());
}


TEST(PipeLine, PipelineHasName) {
    configureTest();
    auto pipeline1 = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(pipeline1.value()->getPipelineName() == "my first testPipeline");
}

TEST(PipeLine, CanLoadTheHelloWordLib) {
    configureTest();
    auto pipeline1 = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01);
    optional<shared_ptr<PipelineStep>> pipelineStep = pipeline1.value()->getStepByName("test Step 02");
    EXPECT_TRUE(pipelineStep.has_value());
    if(pipelineStep.has_value()) {
        PipelineStep* currentStep = pipelineStep.value().get();
        EXPECT_TRUE(currentStep->isInitComplete());
    }
}

TEST(Pipeline, CanExecuteThePipeline) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01);
    testing::internal::CaptureStdout();
    pipeline.value()->execute();
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("Hello World");
    EXPECT_TRUE(regex_search(output, regex));
}


TEST(Pipeline, CanUseArgumentsFromInitDataInJson) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03);
    string testfileName = "./argumentsProcessor_output02.txt";
    filesystem::remove(testfileName);
    pipeline.value()->execute();
    ifstream testFile(testfileName.c_str());
    EXPECT_TRUE(testFile.is_open());
    string line;
    getline(testFile, line);
    EXPECT_EQ("first hello from step 2", line);
    getline(testFile, line);
    EXPECT_EQ("second hello from step 2", line);
    testFile.close();
    filesystem::remove(testfileName);
}

TEST(Pipeline, CanUseProcessData) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03);
    string testfileName = "./argumentsProcessor_output02.txt";
    filesystem::remove(testfileName);
    auto processData = PipelineProcessingData::getInstance();;
    processData->addPayloadData("question", "text/plain", "What is the answer?");
    pipeline.value()->execute(processData);
    optional<shared_ptr<ProcessingPayload>> payload;
    payload = processData->getPayload("question");
    EXPECT_TRUE(payload.has_value());
    payload = processData->getPayload("answer");
    EXPECT_TRUE(payload.has_value());
    EXPECT_EQ(payload.value()->payloadAsString().compare("the answer is 42"), 0);
    filesystem::remove(testfileName);
}

TEST(Pipeline, CanUseBinaryProcessData) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    auto processData = PipelineProcessingData::getInstance();;
    pipeline.value()->execute(processData);
    EXPECT_EQ(1, processData->getCountOfPayloads());
    optional<shared_ptr<ProcessingPayload>> payload;
    payload = processData->getPayload("myBinaryPayloadData");
    EXPECT_TRUE(payload.has_value());
    EXPECT_EQ("ProcessingError",
        ((ProcessingError*)(payload.value()->payloadAsBinaryData().get()))->getErrorCode());    
    EXPECT_EQ("ProcessingError is the only BinaryDataPayload, that currently is supported. "
        "firstArgument: first hello from arguments secondArgument: second hello from arguments",
        ((ProcessingError*)(payload.value()->payloadAsBinaryData().get()))->getErrorMessage());
}

TEST(Pipeline, PipelineCanHaveSelectorPatterns) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key02", "value02");
    EXPECT_EQ(2, pipeline.value()->getCountOfMatchingPatterns());
}

TEST(Pipeline, SelectorPatternCanBeOverwritten) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key02", "value02");
    EXPECT_EQ("value02", pipeline.value()->getMatchingPattern("key02").value());
    pipeline.value()->addMatchingPattern("key02", "value03");
    EXPECT_EQ("value03", pipeline.value()->getMatchingPattern("key02").value());
    EXPECT_EQ(2, pipeline.value()->getCountOfMatchingPatterns());
}

TEST(Pipeline, SelectorPatternCanBeRemoved) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key02", "value02");
    pipeline.value()->removeMatchingPattern("key01");
    EXPECT_FALSE(pipeline.value()->getMatchingPattern("key01").has_value());
    EXPECT_EQ(1, pipeline.value()->getCountOfMatchingPatterns());
}

TEST(Pipeline, RemovingNonExistingSelectorHasNoEffect) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04);
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key02", "value02");
    pipeline.value()->removeMatchingPattern("key03");
    EXPECT_EQ(2, pipeline.value()->getCountOfMatchingPatterns());
}

TEST(Pipeline, ProcessesDataOnlyIfPayloadPatternsMatchThePipelinePatterns) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03);
    string testfileName = "./argumentsProcessor_output02.txt";
    filesystem::remove(testfileName);
    auto processData = PipelineProcessingData::getInstance();;
    processData->addPayloadData("question", "text/plain", "What is the answer?");
    processData->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key01", "value01");
    pipeline.value()->execute(processData);
    optional<shared_ptr<ProcessingPayload>> payload;
    payload = processData->getPayload("answer");
    EXPECT_TRUE(payload.has_value());
    EXPECT_EQ(payload.value()->payloadAsString().compare("the answer is 42"), 0);
    filesystem::remove(testfileName);
}

TEST(Pipeline, DoesNotProcessDataIfMatchingPatternsDiffer) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03);
    string testfileName = "./argumentsProcessor_output02.txt";
    filesystem::remove(testfileName);
    auto processData = PipelineProcessingData::getInstance();;
    processData->addPayloadData("question", "text/plain", "What is the answer?");
    processData->addMatchingPattern("key01", "value01");
    pipeline.value()->addMatchingPattern("key01", "value02");
    pipeline.value()->execute(processData);
    optional<shared_ptr<ProcessingPayload>> payload;
    payload = processData->getPayload("answer");
    EXPECT_FALSE(payload.has_value());
    filesystem::remove(testfileName);
}

TEST(Pipeline, PipelineCanLogErrors) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_05);
    auto processData = PipelineProcessingData::getInstance();;
    pipeline.value()->execute(processData);
    EXPECT_TRUE(processData->hasError());
}

TEST(Pipeline, CanGetAllErrors) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_pipeline::testFilesDir + PIPELINE_CONFIG_TEST_FILE_05);
    auto processData = PipelineProcessingData::getInstance();;
    pipeline.value()->execute(processData);
    vector<shared_ptr<ProcessingError>> allErrors = processData->getAllErrors();
    EXPECT_EQ(2, allErrors.size());
    EXPECT_EQ("first error", allErrors.at(0)->getErrorCode());
    EXPECT_EQ("this is a test error", allErrors.at(0)->getErrorMessage());
    EXPECT_EQ("second error", allErrors.at(1)->getErrorCode());
    EXPECT_EQ("this is yet another test error", allErrors.at(1)->getErrorMessage());
}
