#include <gtest/gtest.h>
#include <filesystem>
#include "logger.h"
#include "pipeline.h"

using namespace std;
using namespace event_forge;

#define PIPELINE_CONFIG_TEST_FILE_07 "/pipelineConfig07.json"

namespace test_fileconnector {
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
    test_fileconnector::configureLogger();
    test_fileconnector::configureTestVariables();
}

TEST(FileConnector, CanSendWithoutAnyFileNameInformation) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_fileconnector::testFilesDir + PIPELINE_CONFIG_TEST_FILE_07);
    auto processData = PipelineProcessingData::getInstance();
    processData->addPayloadData("FILE_SEND_TEXT_DATA", "text/plain", "Hello fileconnector");
    pipeline.value()->execute(processData);

    string fileNameBeginsWith = processData->getMatchingPattern(PAYLOAD_MATCHING_PATTERN_TRANSACTION_ID).value_or("none");
    int filesFound = 0;
    for(auto const& dirEntry : filesystem::directory_iterator{"fileconnectorTest/outDir"}) {
        LOGGER.info("found file: " + string(dirEntry.path().filename()));
        filesystem::remove(dirEntry);
        filesFound++;
        EXPECT_TRUE(string(dirEntry.path().filename()).starts_with(fileNameBeginsWith));
    }
    filesystem::remove("fileconnectorTest/outDir");
    filesystem::remove("fileconnectorTest");
    EXPECT_EQ(1, filesFound);
}

TEST(FileConnector, OutputFileNameCanBeSetViaProperties) {
    configureTest();
    auto pipeline = Pipeline::getInstance(test_fileconnector::testFilesDir + PIPELINE_CONFIG_TEST_FILE_07);
    auto processData = PipelineProcessingData::getInstance();
    processData->addPayloadData("FILE_SEND_TEXT_DATA", "text/plain", "Hello fileconnector");
    processData->addMatchingPattern(PAYLOAD_MATCHING_PATTERN_OUTPUT_FILE_NAME, "outfile");
    pipeline.value()->execute(processData);

    string fileNameBeginsWith = "outfile";
    int filesFound = 0;
    for(auto const& dirEntry : filesystem::directory_iterator{"fileconnectorTest/outDir"}) {
        LOGGER.info("found file: " + string(dirEntry.path().filename()));
        filesystem::remove(dirEntry);
        filesFound++;
        EXPECT_TRUE(string(dirEntry.path().filename()).starts_with(fileNameBeginsWith));
    }
    filesystem::remove("fileconnectorTest/outDir");
    filesystem::remove("fileconnectorTest");
    EXPECT_EQ(1, filesFound);
}
