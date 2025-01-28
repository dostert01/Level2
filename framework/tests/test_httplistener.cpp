#include <gtest/gtest.h>
#include <thread>
#include "logger.h"
#include "pipeline.h"
#include "applicationcontext.h"
#include "payloadnames.h"
#include "httplistener.h"

using namespace std;
using namespace event_forge;

#define PIPELINE_CONFIG_TEST_FILE_06 "/pipelineConfig06.json"
#define APP_CONFIG_TEST_FILE_01 "/applicationConfig01.json"

namespace test_httplistener {
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
    test_httplistener::configureLogger();
    test_httplistener::configureTestVariables();
}

TEST(HTTPListener, CanCreateAnInstaneOfTheHTTPListener) {
    shared_ptr<HTTPListener> listener = HTTPListener::getInstance();
    EXPECT_TRUE(listener.get() != nullptr);
}

TEST(HTTPListener, IsInstanceOfNetworkListener) {
    shared_ptr<HTTPListener> listener = HTTPListener::getInstance();
    NetworkListener* l = dynamic_cast<NetworkListener*>(listener.get());
    EXPECT_TRUE(l != NULL);
}

TEST(HTTPListener, CanBeStoredAsNetworkListener) {
    auto listener = HTTPListener::getInstance();
    HTTPListener* l = dynamic_cast<HTTPListener*>(listener.get());
    EXPECT_TRUE(l != NULL);
}

