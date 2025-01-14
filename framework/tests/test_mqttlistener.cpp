#include <gtest/gtest.h>
#include <thread>
#include "logger.h"
#include "pipeline.h"
#include "applicationcontext.h"
#include "payloadnames.h"
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
    EXPECT_EQ("test/topic01", listeners[0]->getTopic(0).value());
    EXPECT_EQ("test/topic04", listeners[1]->getTopic(1).value());
}

TEST(MQTTListener, CanREturnTopicNamesAsVectorOfStrings) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    vector<string> names = listeners[0]->getTopicNames();
    EXPECT_EQ("test/topic01", names[0]);
    EXPECT_EQ("test/topic02", names[1]);
}

TEST(MQTTListener, initFailsIfConnectionParamsAreNotSet) {
    shared_ptr<NetworkListener> listener = MQTTListener::getInstance();
    listener->init(nullptr);
    EXPECT_FALSE(listener->isIniComplete());
}

TEST(MQTTListener, CanInitFromApplicationConext) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    for(auto listener : listeners) {
        listener->init(nullptr);
        EXPECT_TRUE(listener->isIniComplete());
    }
}

TEST(MQTTListener, IsNotListeningRightAfterInit) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    for(auto listener : listeners) {
        listener->init(nullptr);
        EXPECT_FALSE(listeners[0]->isListening());
    }
}

TEST(MQTTListener, IsListeningAfterCallingStartListening) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    for(auto listener : listeners) {
        listener->init(nullptr);
        listener->startListening();
        EXPECT_TRUE(listeners[0]->isListening());
    }
}

void sendMessage() {
    LOGGER.info("hello from the sending thread");
    MosquittoWrapper mqtt;
    mqtt.init("127.0.0.1", 1883, "testSender", "test/topic02");
    mqtt.sendData("Hello from the test");
}

TEST(MQTTListener, WritesReceivedDataIntoTheQueue) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_mqttlistener::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MQTTListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MQTTListener>("Listeners/MQTTListeners");
    shared_ptr<PipelineFiFo> fifo = PipelineFiFo::getInstance();
    for(auto listener : listeners) {
        listener->init(fifo);
        listener->startListening();
        EXPECT_TRUE(listeners[0]->isListening());
    }
    LOGGER.info("stating thread");
    thread t(sendMessage);
    t.join();
    LOGGER.info("thread joined");
    optional<shared_ptr<PipelineProcessingData>> data  = listeners[0]->getLastMessage();
    int i = 0;
    while(data == nullopt && ++i < 100) {
        data  = listeners[0]->getLastMessage();
        usleep(100000);
    }
    EXPECT_EQ(1, data.value()->getCountOfPayloads());

    std::optional<std::shared_ptr<ProcessingPayload>> payload =
        data.value()->getPayload(PAYLOAD_NAME_MQTT_RECEIVED_DATA);
    EXPECT_EQ("Hello from the test", payload.value()->payloadAsString());

}
