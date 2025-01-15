#include <gtest/gtest.h>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>
#include "pipelineprocessor.h"
#include "../logger/logger.h"

using namespace event_forge;

#ifndef LOGGER
#define LOGGER Logger::getInstance()
#endif

#define PROCESS_CONFIG_TEST_FILE_01 "/processConfig01.json"

namespace test_pipeline_processor {
    std::string workingDir;
    std::string testFilesDir;

    void configureLogger() {
        LOGGER.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
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
        EXPECT_EQ(3, processor.value()->getCountOfPipelines());
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
    std::optional<std::shared_ptr<Pipeline>> pipeline = prc->getPipelineByName("my first testPipeline");
    EXPECT_TRUE(pipeline.has_value());
    EXPECT_EQ("my first testPipeline", pipeline.value().get()->getPipelineName());
}

TEST(PipeLineProcessor, PipelineHasMatchingPatterns) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    std::shared_ptr<Pipeline> pipeline = processor.value().get()->getPipelineByName("my first testPipeline").value();
    EXPECT_EQ(5, pipeline.get()->getCountOfMatchingPatterns());
}

TEST(PipeLineProcessor, OneOfThreePipelinesProcessesTheMatchingPayload) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    PipelineProcessingData processData;
    processData.addPayloadData("question", "text/plain", "What is the answer?");
    processData.addMatchingPattern("key01", "value01");
    processData.addMatchingPattern("key02", "value02");
    processData.addMatchingPattern("key03", "value03");
    processData.addMatchingPattern("key04", "value04");
    processData.addMatchingPattern("key05", "value05");
    processor.value().get()->execute(processData);
    EXPECT_EQ(1, processData.getProcessingCounter());
    EXPECT_EQ("my first testPipeline", processData.getLastProcessedPipelineName());
}

TEST(PipeLineProcessor, NoneOfThreePipelinesProcessesTheMatchingPayload) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    PipelineProcessingData processData;
    processData.addPayloadData("question", "text/plain", "What is the answer?");
    processData.addMatchingPattern("thisPattern", "does not match any pipeline");
    processor.value().get()->execute(processData);
    EXPECT_EQ(0, processData.getProcessingCounter());
    EXPECT_EQ("", processData.getLastProcessedPipelineName());
}

TEST(PipeLineProcessor, TwoOfThreePipelinesProcessesThePayloadWithoutAnyMatchingPattern) {
    configureTest();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    PipelineProcessingData processData;
    processData.addPayloadData("question", "text/plain", "What is the answer?");
    processor.value().get()->execute(processData);
    EXPECT_EQ(2, processData.getProcessingCounter());
    EXPECT_EQ("my pipeline with binaryData", processData.getLastProcessedPipelineName());
}

TEST(PipeLineProcessor, CanStartTheProcessingLoop) {
    configureTest();

    PipelineProcessingData processData;
    processData.addPayloadData("question", "text/plain", "What is the answer?");

    shared_ptr<PipelineFiFo> fifo = PipelineFiFo::getInstance();

    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    processor.value()->startProcessingLoop(fifo);
    EXPECT_TRUE(processor.value()->isProcessingLoopRunning());
    sleep(1);
    processor.value()->stopProcessingLoop();
    EXPECT_FALSE(processor.value()->isProcessingLoopRunning());

}

TEST(PipeLineProcessor, ProcessingLoopProcessesSomeData) {
    configureTest();

    shared_ptr<PipelineProcessingData> processData = make_shared<PipelineProcessingData>();
    processData->addPayloadData("question", "text/plain", "What is the answer?");
    shared_ptr<PipelineFiFo> fifo = PipelineFiFo::getInstance();
    std::optional<std::unique_ptr<PipeLineProcessor>> processor = PipeLineProcessor::getInstance(test_pipeline_processor::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    processor.value()->startProcessingLoop(fifo);
    sleep(1);
    fifo->enqueue(processData);
    sleep(1);
    EXPECT_EQ(2, processData->getProcessingCounter());
    EXPECT_EQ("my pipeline with binaryData", processData->getLastProcessedPipelineName());
    processor.value()->stopProcessingLoop();
    sleep(1);
}