#include <gtest/gtest.h>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "logger.h"
#include "pipeline.h"
#include "applicationcontext.h"
#include "httplistener.h"
#include "http11.h"

#define HALF_A_SECOND 500000

using namespace std;
using namespace event_forge;

#define APP_CONFIG_TEST_FILE_02 "/applicationConfig02.json"
#define APP_CONFIG_TEST_FILE_05 "/applicationConfig05.json"
#define HTTP_REQUEST_TEST_FILE_01 "/httpRequest01.txt"
#define HTTP_REQUEST_TEST_FILE_02 "/httpRequest02.txt"

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

    int sendHelloWorld() {
        string command = "echo \"hello world\" | nc -N 127.0.0.1 8889";
        FILE *file = popen(command.c_str(), "r");
        string result = "";
        int returnValue = -1;
        if(file) {
            LOGGER.info("popen returned valid file pointer");
            char buffer[2048];
            while(fgets(buffer, 2048, file) != NULL) {
                LOGGER.info("reding result ...");
                result.append(buffer);
            }
            returnValue = pclose(file);
        }
        LOGGER.info("result returned by call to " + command + " : '" + result + "'");
        return WEXITSTATUS(returnValue);
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

TEST(HTTPListener, ListenerCanBeCreatedThroughAppContext) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_httplistener::testFilesDir + APP_CONFIG_TEST_FILE_02);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<HTTPListener>("Listeners/HTTPListeners");
    EXPECT_EQ(1, listeners.size());
    EXPECT_EQ(8899, listeners[0]->getPort());
}

TEST(HTTPListener, isListeningStatusWorks01) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_httplistener::testFilesDir + APP_CONFIG_TEST_FILE_02);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<HTTPListener>("Listeners/HTTPListeners");
    EXPECT_FALSE(listeners[0]->isListening());
}

TEST(HTTPListener, isListeningStatusWorks02) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_httplistener::testFilesDir + APP_CONFIG_TEST_FILE_02);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<HTTPListener>("Listeners/HTTPListeners");
    listeners[0]->init(PipelineFiFo::getInstance());
    listeners[0]->startListening();
    EXPECT_TRUE(listeners[0]->isListening());
}

TEST(HTTPListener, CanReceiveSomeData) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_httplistener::testFilesDir + APP_CONFIG_TEST_FILE_05);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<HTTPListener>("Listeners/HTTPListeners");
    EXPECT_EQ(1, listeners.size());
    auto fifo = PipelineFiFo::getInstance();
    for(auto listener : listeners) {
        listener->init(fifo);
        listener->startListening();
    }
    usleep(HALF_A_SECOND);
    EXPECT_EQ(0, test_httplistener::sendHelloWorld());
}

TEST(Http11, canParseFirstLine) {
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_01).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http;
    auto request = http.readRequest(fd, "");
    EXPECT_EQ("GET", request.value()->getMethod());
    EXPECT_EQ("/api/1.0/index.html", request.value()->getPath());
}

TEST(Http11, canParseNormalHeaderLines) {
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_01).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http;
    auto request = http.readRequest(fd, "");
    EXPECT_EQ("www.example.com", request.value()->getHeaderFieldValue("Host"));
    EXPECT_EQ("curl/7.68.0", request.value()->getHeaderFieldValue("User-Agent"));
    EXPECT_EQ("*/*", request.value()->getHeaderFieldValue("Accept"));    
}

TEST(Http11, parsingInvalidHeaderFailsWithEmptyRequestObject) {
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_02).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http;
    auto request = http.readRequest(fd, "");
    EXPECT_EQ(std::nullopt, request);
}
