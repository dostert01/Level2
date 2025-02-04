#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>
#include "pipelineprocessor.h"
#include "../logger/logger.h"

using namespace level2;

#ifndef LOGGER
#define LOGGER Logger::getInstance()
#endif

#define PROCESS_CONFIG_TEST_FILE_01 "/processConfig01.json"
#define PROCESS_CONFIG_TEST_FILE_02 "/processConfig02.json"

using namespace std;

namespace test_pipeline_processor {
    string workingDir;
    string testFilesDir;

    void configureLogger() {
        LOGGER.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
        LOGGER.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
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
    test_pipeline_processor::configureLogger();
    test_pipeline_processor::configureTestVariables();
}

TEST(PipeLineProcessor, ReturnsEmptyOptionalIfConfigFileNotPresent) {
    configureTest();
    auto processor = PipeLineProcessor::getInstance("noFileHere");
    EXPECT_FALSE(processor.has_value());
}

TEST(PipeLineProcessor, CanLoadConfigFromJson) {
    configureTest();
    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    EXPECT_TRUE(processor.has_value());
    if(processor.has_value())
        EXPECT_EQ(4, processor.value()->getCountOfPipelines());
}

TEST(PipeLineProcessor, ProcessHasAName) {
    configureTest();
    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    EXPECT_EQ("my first testProcess", processor.value()->getProcessName());
}

TEST(PipeLineProcessor, CanGetPipelineByName) {
    configureTest();
    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    PipeLineProcessor* prc = processor.value().get();
    auto pipeline = prc->getPipelineByName("my first testPipeline");
    EXPECT_TRUE(pipeline.has_value());
    EXPECT_EQ("my first testPipeline", pipeline.value().get()->getPipelineName());
}

TEST(PipeLineProcessor, PipelineHasMatchingPatterns) {
    configureTest();
    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    shared_ptr<Pipeline> pipeline = processor.value().get()->getPipelineByName("my first testPipeline").value();
    EXPECT_EQ(5, pipeline.get()->getCountOfMatchingPatterns());
}

TEST(PipeLineProcessor, ThreeOutOfFourPipelinesAreTheMatchingThePayload) {
    configureTest();
    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    shared_ptr<PipelineProcessingData> processData = PipelineProcessingData::getInstance();
    processData->addPayloadData("question", "text/plain", "What is the answer?");
    processData->addMatchingPattern("key01", "value01");
    processData->addMatchingPattern("key02", "value02");
    processData->addMatchingPattern("key03", "value03");
    processData->addMatchingPattern("key04", "value04");
    processData->addMatchingPattern("key05", "value05");
    processor.value().get()->execute(processData);
    EXPECT_EQ(3, processData->getProcessingCounter());
    EXPECT_EQ("my pipeline with binaryData", processData->getLastProcessedPipelineName());
}

TEST(PipeLineProcessor, NoneMatchingPayloadIsProcessedByPipelinesWithoutAnyMatchingPatterns) {
    configureTest();
    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    shared_ptr<PipelineProcessingData> processData = PipelineProcessingData::getInstance();
    processData->addPayloadData("question", "text/plain", "What is the answer?");
    processData->addMatchingPattern("thisPattern", "does not match any pipeline");
    processor.value().get()->execute(processData);
    EXPECT_EQ(2, processData->getProcessingCounter());
    EXPECT_EQ("my pipeline with binaryData", processData->getLastProcessedPipelineName());
}

TEST(PipeLineProcessor, TwoOfThreePipelinesProcessesThePayloadWithoutAnyMatchingPattern) {
    configureTest();
    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    shared_ptr<PipelineProcessingData> processData = PipelineProcessingData::getInstance();
    processData->addPayloadData("question", "text/plain", "What is the answer?");
    processor.value().get()->execute(processData);
    EXPECT_EQ(2, processData->getProcessingCounter());
    EXPECT_EQ("my pipeline with binaryData", processData->getLastProcessedPipelineName());
}

TEST(PipeLineProcessor, CanStartTheProcessingLoop) {
    configureTest();

    shared_ptr<PipelineProcessingData> processData = PipelineProcessingData::getInstance();
    processData->addPayloadData("question", "text/plain", "What is the answer?");

    shared_ptr<PipelineFiFo> fifo = PipelineFiFo::getInstance();

    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    processor.value()->startProcessingLoop(fifo);
    EXPECT_TRUE(processor.value()->isProcessingLoopRunning());
    sleep(1);
    processor.value()->stopProcessingLoop();
    EXPECT_FALSE(processor.value()->isProcessingLoopRunning());

}

TEST(PipeLineProcessor, ProcessingLoopProcessesSomeData) {
    configureTest();

    auto processData = PipelineProcessingData::getInstance();
    processData->addPayloadData("question", "text/plain", "What is the answer?");
    auto fifo = PipelineFiFo::getInstance();
    auto processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_02);
    processor.value()->startProcessingLoop(fifo);
    sleep(1);
    fifo->enqueue(processData);
    sleep(1);
    EXPECT_EQ(1, processData->getProcessingCounter());
    EXPECT_EQ("my pipeline with binaryData", processData->getLastProcessedPipelineName());
    auto payload = processData->getPayload("myBinaryPayloadData");
    auto data = payload.value()->payloadAsBinaryData();
    EXPECT_EQ("ProcessingError is the only BinaryDataPayload, that currently is supported. "
        "firstArgument: first hello from arguments secondArgument: second hello from arguments",
        dynamic_pointer_cast<ProcessingError>(data)->getErrorMessage());
    processor.value()->stopProcessingLoop();
    sleep(1);
}