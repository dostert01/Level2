#include <gtest/gtest.h>
#include <regex>
#include <thread>
#include "pipeline.h"
#include "../logger/logger.h"

using namespace event_forge;
using namespace std;


namespace test_apistructs {
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
    test_apistructs::configureLogger();
    test_apistructs::configureTestVariables();
}

bool regexMatch(shared_ptr<PipelineProcessingData> data, string propertyName, string regexPattern) {
    string s = data->getMatchingPattern(
        propertyName).value_or("");
    LOGGER.info(string(propertyName) + ": " + s);
    regex regex(regexPattern.c_str());
    return regex_match(s, regex);
}

TEST(ApiStructs, PipelineProcessingDataHasDefaultProperties) {
    auto data = PipelineProcessingData::getInstance();

    EXPECT_TRUE(regexMatch(data,
        PAYLOAD_MATCHING_PATTERN_TRANSACTION_ID, "^[0-9]{14}_[0]{9}"));
    EXPECT_TRUE(regexMatch(data,
        PAYLOAD_MATCHING_PATTERN_DATE_CREATED, "^[0-9]{8}"));
    EXPECT_TRUE(regexMatch(data,
        PAYLOAD_MATCHING_PATTERN_TIME_CREATED, "^[0-9]{6}"));
}

void create1000TransactionIds() {
    for(int i=0; i<1000; i++) {
        auto data = PipelineProcessingData::getInstance();    
    }
}

TEST(ApiStructs, CreationOfTransactionIdsIsThreadSafe) {
    vector<shared_ptr<thread>> threads;
    for(int i=0; i<100; i++) {
        threads.push_back(make_shared<thread>(create1000TransactionIds));
    }
    for(auto t : threads) {
        t->join();
    }
    auto data = PipelineProcessingData::getInstance();
    EXPECT_TRUE(regexMatch(data,
        PAYLOAD_MATCHING_PATTERN_TRANSACTION_ID, "^[0-9]{14}_000100001"));
}

TEST(ProcessingPayload, hasPayloadName) {
    ProcessingPayload payload("name of payload", "mime/type", "I'm the payload");
    EXPECT_EQ("name of payload", payload.getPayloadName());
}
