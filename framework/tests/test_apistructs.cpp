#include <gtest/gtest.h>

#include <iostream>
#include <regex>
#include <thread>
#include <nlohmann/json.hpp>

#include "../logger/logger.h"
#include "pipeline.h"

using namespace event_forge;
using namespace std;
using json = nlohmann::json;

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

TEST(ProcessingError, canWriteItselfToJson) {
    ProcessingError error1("error code 01", "error message 01");
    ProcessingError error2("error code 02", "error message 02");
    json j = {};
    error1.toJson(&j);
    error2.toJson(&j);
    EXPECT_EQ("[{\"errorCode\":\"error code 01\",\"errorMessage\":\"error message 01\"},{\"errorCode\":\"error code 02\",\"errorMessage\":\"error message 02\"}]", j.dump());
    std::cout << std::setw(4) << j << '\n';
}

TEST(ProcessingError, canWriteItselfToJson02) {
    ProcessingError data1 = ProcessingError("code", "message");
    json j;
    data1.toJson(&j);    
    EXPECT_EQ("[{\"errorCode\":\"code\",\"errorMessage\":\"message\"}]", j.dump());
    std::cout << std::setw(4) << j << '\n';
}


TEST(ProcessingPayload, canWriteItselfToJson) {
    ProcessingPayload payload1("Payload01", "text/plain", "some data");
    ProcessingPayload payload2("Payload02", "text/plain", "some more data");
    json j = {};
    payload1.toJson(&j);
    payload2.toJson(&j);
    EXPECT_EQ("[{\"mimetype\":\"text/plain\",\"parameters\":{},\"payload\":\"c29tZSBkYXRh\",\"payloadName\":\"Payload01\"},{\"mimetype\":\"text/plain\",\"parameters\":{},\"payload\":\"c29tZSBtb3JlIGRhdGE=\",\"payloadName\":\"Payload02\"}]", j.dump());
    std::cout << std::setw(4) << j << '\n';
}


TEST(PipelineProcessingData, canWriteItselfToJson) {
    auto data1 = PipelineProcessingData::getInstance();
    data1->addPayloadData("payload 01", "text/plain", "some text");
    data1->addPayloadData("payload 02", "text/plain", "some more text");
    data1->addError("errorcode 0815", "just an ordinary error");
    data1->addError("42", "unfortunately, we have forgotten the question");
    data1->addError("0", "no error here ;-P");
    json j = {};
    data1->toJson(&j);
    EXPECT_EQ(850, j.dump().length());
    std::cout << std::setw(4) << j << '\n';
}



