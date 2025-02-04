#include <gtest/gtest.h>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <errno.h>
#include <fstream>
#include <string>
#include <iostream>

#include "logger.h"
#include "pipeline.h"
#include "applicationcontext.h"
#include "payloadnames.h"
#include "filelistener.h"

using namespace std;
using namespace level2;

#define PIPELINE_CONFIG_TEST_FILE_06 "/pipelineConfig06.json"
#define APP_CONFIG_TEST_FILE_03 "/applicationConfig03.json"
#define APP_CONFIG_TEST_FILE_04 "/applicationConfig04.json"
#define TEST_MONITORING_ROOT_DIRECTORY "filelistenerTest.tmp"

#define HALF_A_SECOND 500000

namespace test_filelistener {
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

    void removeDirectory (string path) {
        try {
            filesystem::remove_all(path);
        } catch (const exception& e) {
            LOGGER.warn("failed to delete directory '" + path + "' : " + e.what());
        }
    }

    void writeToFile(string fileName, string fileContent) {
        ofstream outFile(fileName);
        outFile << fileContent;
    }
    
    bool directoryExists(string path) {
        struct stat fileStatus;
        return (stat(path.c_str(), &fileStatus) == 0 && (fileStatus.st_mode & S_IFDIR));
    }

}

void configureTest() {
    test_filelistener::configureLogger();
    test_filelistener::configureTestVariables();
    test_filelistener::removeDirectory(TEST_MONITORING_ROOT_DIRECTORY);
}

TEST(FileListener, ListenerCanBeCreatedThroughAppContext) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_filelistener::testFilesDir + APP_CONFIG_TEST_FILE_03);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<FileListener>("Listeners/FileListeners");
    EXPECT_EQ(2, listeners.size());
    EXPECT_EQ("filelistenerTest.tmp/newFiles01", listeners[0]->getDirectory2Monitor());
    EXPECT_EQ("filelistenerTest.tmp/newFiles02", listeners[1]->getDirectory2Monitor());
}

TEST(FileListener, CreatedMonitoringDirectoriesIfTheyDoNotExists) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_filelistener::testFilesDir + APP_CONFIG_TEST_FILE_03);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<FileListener>("Listeners/FileListeners");
    auto fifo = PipelineFiFo::getInstance();
    for(auto listener : listeners) {
        listener->init(fifo);
        EXPECT_TRUE(test_filelistener::directoryExists(listener->getDirectory2Monitor()));
    }
    test_filelistener::removeDirectory(TEST_MONITORING_ROOT_DIRECTORY);
}

TEST(FileListener, MarksInitAsNotCompleteIfDirectoriesDoNotExistsAndCanNotBeCreated) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_filelistener::testFilesDir + APP_CONFIG_TEST_FILE_04);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<FileListener>("Listeners/FileListeners");
    auto fifo = PipelineFiFo::getInstance();
    for(auto listener : listeners) {
        listener->init(fifo);
        EXPECT_FALSE(listener->isIniComplete());
    }
}

TEST(FileListener, DeliversFileContentToTheFifo) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_filelistener::testFilesDir + APP_CONFIG_TEST_FILE_03);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<FileListener>("Listeners/FileListeners");
    auto fifo = PipelineFiFo::getInstance();
    for(auto listener : listeners) {
        listener->init(fifo);
        listener->startListening();
    }
    test_filelistener::writeToFile("filelistenerTest.tmp/newFiles01/testFile01.txt", "Hello from testFile01.txt");
    usleep(HALF_A_SECOND * 2);
    test_filelistener::writeToFile("filelistenerTest.tmp/newFiles01/testFile02.txt", "Hello from testFile02.txt");
    test_filelistener::writeToFile("filelistenerTest.tmp/newFiles02/testFile03.txt", "Hello from testFile03.txt");
    int i=0;
    while(fifo->getCountOfElementsInFifo() < 3 && i++ < 10) {
        usleep(HALF_A_SECOND);
    }
    EXPECT_EQ(3, fifo->getCountOfElementsInFifo());
    
    vector<shared_ptr<PipelineProcessingData>> results;
    results.push_back(fifo->dequeue().value());
    results.push_back(fifo->dequeue().value());
    results.push_back(fifo->dequeue().value());
    for(auto result : results) {
        LOGGER.info("result:" + result->getLastPayload().value()->payloadAsString());
        EXPECT_TRUE(result->getLastPayload().value()->payloadAsString().find(
            result->getMatchingPattern(PAYLOAD_MATCHING_PATTERN_INPUT_FILE_NAME).value_or("not found")) != string::npos);
    }    
    test_filelistener::removeDirectory(TEST_MONITORING_ROOT_DIRECTORY);
}

TEST(FileListener, HasAllRequiresMatchingPatterns) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_filelistener::testFilesDir + APP_CONFIG_TEST_FILE_03);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<FileListener>("Listeners/FileListeners");
    auto fifo = PipelineFiFo::getInstance();
    for(auto listener : listeners) {
        listener->init(fifo);
        listener->startListening();
    }
    test_filelistener::writeToFile("filelistenerTest.tmp/newFiles01/testFile01.txt", "Hello from testFile01.txt");
    int i=0;
    while(fifo->getCountOfElementsInFifo() < 1 && i++ < 10) {
        usleep(HALF_A_SECOND);
    }
    
    auto result = fifo->dequeue().value();
    EXPECT_EQ("filelistenerTest.tmp/newFiles01", result->getMatchingPattern(PAYLOAD_MATCHING_PATTERN_DATA_ORIGIN).value_or("not found"));
    EXPECT_EQ(PAYLOAD_MATCHING_PATTERN_VALUE_FILE_LISTENER, result->getMatchingPattern(PAYLOAD_MATCHING_PATTERN_RECEIVED_BY_LISTENER).value_or("not found"));
    EXPECT_EQ(PAYLOAD_MATCHING_PATTERN_VALUE_PROTOCOL_FILE, result->getMatchingPattern(PAYLOAD_MATCHING_PATTERN_RECEIVED_VIA_PROTOCOL).value_or("not found"));
    EXPECT_EQ("testFile01.txt", result->getMatchingPattern(PAYLOAD_MATCHING_PATTERN_INPUT_FILE_NAME).value_or("not found"));

    test_filelistener::removeDirectory(TEST_MONITORING_ROOT_DIRECTORY);
}
