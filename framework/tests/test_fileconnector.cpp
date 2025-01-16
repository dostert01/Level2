#include <gtest/gtest.h>
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
    EXPECT_EQ(1, 1);
}

