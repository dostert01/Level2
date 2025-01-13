#include <gtest/gtest.h>
#include "logger.h"
#include "pipeline.h"
#include "applicationcontext.h"

#include "mqttlistener.h"

using namespace std;
using namespace event_forge;

#define PIPELINE_CONFIG_TEST_FILE_06 "/pipelineConfig06.json"
#define APP_CONFIG_TEST_FILE_01 "/applicationConfig01.json"

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

TEST(MQTTListener, ListenerCanBeCreatedThroughAppContext) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    EXPECT_EQ(2, listeners.size());
    EXPECT_EQ("test/mqttconnector01", listeners[0]->getTopic());
    EXPECT_EQ("test/mqttconnector02", listeners[1]->getTopic());
}

TEST(MQTTListener, initFailsIfConnectionParamsAreNotSet) {
    shared_ptr<NetworkListener> listener = MQTTListener::getInstance();
    listener->init();
    EXPECT_FALSE(listener->isIniComplete());
}

TEST(MQTTListener, CanInitFromApplicationConext) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    for(auto listener : listeners) {
        listener->init();
        EXPECT_TRUE(listener->isIniComplete());
    }
}

TEST(MQTTListener, IsNotListeningRightAfterInit) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    for(auto listener : listeners) {
        listener->init();
        EXPECT_FALSE(listeners[0]->isListening());
    }
}

TEST(MQTTListener, IsListeningAfterCallingStartListening) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    for(auto listener : listeners) {
        listener->init();
        listener->startListening();
        EXPECT_TRUE(listeners[0]->isListening());
    }
}
