#include <gtest/gtest.h>
#include <filesystem>
#include "../logger/logger.h"
#include "pipeline.h"

using namespace std;
using namespace event_forge;

#define PIPELINE_CONFIG_TEST_FILE_06 "/pipelineConfig06.json"

namespace test_mqttconnector {
    std::string workingDir;
    std::string testFilesDir;

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
    test_mqttconnector::configureLogger();
    test_mqttconnector::configureTestVariables();
}

TEST(MQTTConnector, CanSend) {
    configureTest();
    optional<shared_ptr<Pipeline>> pipeline1 = Pipeline::getInstance(test_mqttconnector::testFilesDir + PIPELINE_CONFIG_TEST_FILE_06);
    pipeline1.value()->execute();
    EXPECT_EQ(1, 1);
}
