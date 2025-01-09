#include <gtest/gtest.h>
#include "logger.h"
#include "pipeline.h"
#include "mqttlistener.h"

using namespace std;
using namespace event_forge;

#define PIPELINE_CONFIG_TEST_FILE_06 "/pipelineConfig06.json"

namespace test_mqttlistener {
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
    test_mqttlistener::configureLogger();
    test_mqttlistener::configureTestVariables();
}

TEST(MQTTListener, CanCreateAnInstaneOfTheMQTTListener) {
    shared_ptr<MQTTListener> listener = MQTTListener::getInstance();
    EXPECT_TRUE(listener.get() != nullptr);
}

TEST(MQTTListener, IsInstanceOfNetworkListener) {
    shared_ptr<MQTTListener> listener = MQTTListener::getInstance();
    NetworkListener* l = dynamic_cast<NetworkListener*>(listener.get());
    EXPECT_TRUE(l != NULL);
}

TEST(MQTTListener, CanBeStoredAsNetworkListener) {
    shared_ptr<NetworkListener> listener = MQTTListener::getInstance();
    MQTTListener* l = dynamic_cast<MQTTListener*>(listener.get());
    EXPECT_TRUE(l != NULL);
}

TEST(MQTTListener, CanStartListening) {
    shared_ptr<NetworkListener> listener = MQTTListener::getInstance();
    listener->startListening();
    EXPECT_TRUE(listener->isListening());
}
