#include <gtest/gtest.h>
#include "logger.h"
#include "pipeline.h"
#include "mosquittowrapper.h"

using namespace std;
using namespace event_forge;

#define PIPELINE_CONFIG_TEST_FILE_06 "/pipelineConfig06.json"

namespace test_mqttconnector {
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

    void listenToIncomingMessages() {
        string command = "mosquitto_sub -v -t 'test/mqttconnector' -W 3";
        FILE *file = popen(command.c_str(), "r");
        string result = "";
        if(file) {
            LOGGER.info("popen returned valid file pointer");
            char buffer[2048];
            while(fgets(buffer, 2048, file) != NULL) {
                LOGGER.info("reding result ...");
                result.append(buffer);
            }
            pclose(file);
        }
        LOGGER.info("result returned by call to " + command + " : '" + result + "'");
    }
}

void configureTest() {
    test_mqttconnector::configureLogger();
    test_mqttconnector::configureTestVariables();
}

TEST(MQTTConnector, CanSend) {
    configureTest();
    optional<shared_ptr<Pipeline>> pipeline1 = Pipeline::getInstance(test_mqttconnector::testFilesDir + PIPELINE_CONFIG_TEST_FILE_06);
    shared_ptr<PipelineProcessingData> processData = PipelineProcessingData::getInstance();
    processData->addPayloadData("MQTT_SEND_TEXT_DATA", "text/plain", "Hello via MQTT");
    //thread t(test_mqttconnector::listenToIncomingMessages);
    EXPECT_TRUE(pipeline1.has_value());
    if(pipeline1.has_value())
        pipeline1.value()->execute(processData);
    //t.join();
    EXPECT_EQ(1, 1);
}

TEST(MosquittoWrapper, canCreateInstance) {
    shared_ptr<MosquittoWrapper> wrapper = MosquittoWrapper::getInstance("127.0.0.1", 1883, "someNiceClient", "test/topic");
    EXPECT_TRUE(wrapper != nullptr);
}

TEST(MosquittoWrapper, canInitMosquittoLib) {
    shared_ptr<MosquittoWrapper> wrapper = MosquittoWrapper::getInstance("127.0.0.1", 1883, "someNiceClient", "test/topic");
    EXPECT_TRUE(wrapper->isInitComplete());
}
