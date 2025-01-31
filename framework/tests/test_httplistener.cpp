#include <gtest/gtest.h>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <nlohmann/json.hpp>

#include "logger.h"
#include "pipeline.h"
#include "applicationcontext.h"
#include "httplistener.h"
#include "http11.h"
#include "httpdefs.h"

using json = nlohmann::json;

#define HALF_A_SECOND 500000

using namespace std;
using namespace event_forge;

#define PROCESS_CONFIG_TEST_FILE_01 "/processConfig01.json"
#define PROCESS_CONFIG_TEST_FILE_03 "/processConfig03.json"
#define APP_CONFIG_TEST_FILE_02 "/applicationConfig02.json"
#define APP_CONFIG_TEST_FILE_05 "/applicationConfig05.json"
#define HTTP_REQUEST_TEST_FILE_01 "/httpRequest01.txt"
#define HTTP_REQUEST_TEST_FILE_02 "/httpRequest02.txt"
#define HTTP_REQUEST_TEST_FILE_03 "/httpRequest03.txt"
#define HTTP_REQUEST_TEST_FILE_04 "/httpRequest04.txt"
#define HTTP_REQUEST_TEST_FILE_05 "/httpRequest05.txt"
#define HTTP_REQUEST_TEST_FILE_06 "/httpRequest06.txt"

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
                LOGGER.info("reading result: " + std::string(buffer));
                result.append(buffer);
            }
            returnValue = pclose(file);
        }
        LOGGER.info("result returned by call to " + command + " : '" + result + "'");
        return WEXITSTATUS(returnValue);
    }

   int sendFile(string filePath, std::string& stdOutData) {
        string command = "cat " + filePath + " | nc -N 127.0.0.1 8889";
        FILE *file = popen(command.c_str(), "r");
        string result = "";
        int returnValue = -1;
        if(file) {
            LOGGER.info("popen returned valid file pointer");
            char buffer[2048];
            while(fgets(buffer, 2048, file) != NULL) {
                LOGGER.info("reading result: " + std::string(buffer));
                result.append(buffer);
            }
            returnValue = pclose(file);
        }
        LOGGER.info("result returned by call to " + command + " : '" + result + "'");
        stdOutData = result;
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
    Http11 http(fd);
    auto request = http.readRequest("");
    EXPECT_EQ("GET", request.value()->getMethod());
    EXPECT_EQ("/api/1.0/index.html", request.value()->getPath());
}

TEST(Http11, canParseNormalHeaderLines) {
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_01).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    auto request = http.readRequest("");
    EXPECT_EQ("www.example.com", request.value()->getHeaderFieldValue("Host"));
    EXPECT_EQ("curl/7.68.0", request.value()->getHeaderFieldValue("User-Agent"));
}

TEST(Http11, parsingInvalidHeaderFailsWithEmptyRequestObject) {
    configureTest();
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_02).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    auto request = http.readRequest("");
    EXPECT_EQ(std::nullopt, request);
}

TEST(Http11, getHeaderFieldValueReturnsEmptyOptionalOnFailure) {
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_01).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    auto request = http.readRequest("");
    EXPECT_EQ(std::nullopt, request.value()->getHeaderFieldValue("ThisIsNotPresentInTheHeader"));
}

TEST(Http11, canReturnAllHeaderFields) {
    configureTest();
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_01).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    auto request = http.readRequest("");
    EXPECT_TRUE(request.has_value());
    auto headerFields = request.value()->getAllHeaderFields();
    EXPECT_EQ(3, headerFields->size());
}

TEST(Http11, canParseUrlParams) {
    configureTest();
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_03).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    auto request = http.readRequest("");
    EXPECT_EQ("/api/1.0/index.html", request.value()->getPath());
    EXPECT_EQ(6, request.value()->getCountOfUrlParams());
    auto params = request.value()->getUrlParams("q");
    for(auto param = params.first; param != params.second; ++param) {
        LOGGER.info("found param key='" + param->first + "' value='" + param->second + "'");
        EXPECT_EQ("hello%20world", param->second);
    }
    params = request.value()->getUrlParams("tag");
    int i=0;
    for(auto param = params.first; param != params.second; ++param) {
        LOGGER.info("found param key='" + param->first + "' value='" + param->second + "'");
        EXPECT_TRUE(param->second.starts_with("value0"));
        i++;
    }
    EXPECT_EQ(2, i);
}

TEST(Http11, canGetAllParams) {
    configureTest();
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_03).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    auto request = http.readRequest("");
    std::string result;
    std::shared_ptr<event_forge::UrlParamsMultiMap> params = request.value()->getAllUrlParams();
    for(auto param = params->begin(); param != params->end(); ++param) {
        LOGGER.info("found param key='" + param->first + "' value='" + param->second + "'");
        result.append(param->first + "=" + param->second + " ");
    }
    EXPECT_EQ("category=C%2B%2B emptyParam= page=1 q=hello%20world tag=value01 tag=value02 ", result);
}

TEST(Http11, canGetParamValuesAsVector) {
    configureTest();
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_03).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    auto request = http.readRequest("");
    auto values = request.value()->getUrlParamValues("tag");
    EXPECT_EQ(2, values->size());
    EXPECT_EQ("value01", values->at(0));
    EXPECT_EQ("value02", values->at(1));
}

TEST(Http11, canGetContentLength) {
    configureTest();
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_04).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    auto request = http.readRequest("");
    auto value = request.value()->getContentLength();
    EXPECT_FALSE(value == std::nullopt);
    EXPECT_EQ(5536, value.value());
}

TEST(Http11, canReadLargeData) {
    configureTest();
    int fd = open(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_04).c_str(), O_RDONLY, O_NONBLOCK);
    Http11 http(fd);
    http.readRequest("");
    EXPECT_EQ(5745, http.getBytesRead());
}

TEST(HTTPListener, canReceiveLargeData) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_httplistener::testFilesDir + APP_CONFIG_TEST_FILE_05);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<HTTPListener>("Listeners/HTTPListeners");
    EXPECT_EQ(1, listeners.size());
    auto processor = PipeLineProcessor::getInstance(test_httplistener::testFilesDir + PROCESS_CONFIG_TEST_FILE_01);
    
    listeners[0]->init(processor.value());
    listeners[0]->startListening();
    
    usleep(HALF_A_SECOND);
    std::string stdOut;
    EXPECT_EQ(0, test_httplistener::sendFile(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_04), stdOut));
    usleep(HALF_A_SECOND * 4);

    auto processedData = listeners[0]->getLastProcessingData();
    json j = {};
    processedData.value()->toJson(&j);
    EXPECT_EQ(11195, j.dump().length());
    std::cout << std::setw(4) << j << '\n';
}

TEST(HTTPListener, sendsDataBackOnGetRequest) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_httplistener::testFilesDir + APP_CONFIG_TEST_FILE_05);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<HTTPListener>("Listeners/HTTPListeners");
    EXPECT_EQ(1, listeners.size());
    auto processor = PipeLineProcessor::getInstance(test_httplistener::testFilesDir + PROCESS_CONFIG_TEST_FILE_03);
    listeners[0]->init(processor.value());
    listeners[0]->startListening();

    usleep(HALF_A_SECOND);
    std::string stdOut;
    EXPECT_EQ(0, test_httplistener::sendFile(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_05), stdOut));
    usleep(HALF_A_SECOND * 4);

    auto processedData = listeners[0]->getLastProcessingData();
    json j = {};
    processedData.value()->toJson(&j);
    EXPECT_EQ("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 11\r\nContent-Type: text/plain; charset=utf-8\r\nServer: level2 httpListener\r\n\r\n\r\nhello world", stdOut);
    std::cout << std::setw(4) << j << '\n';
}

TEST(HTTPListener, returnsHttpErrorIfNoProcessingPipelineWasFound) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_httplistener::testFilesDir + APP_CONFIG_TEST_FILE_05);
    auto listeners = APP_CONTEXT.createObjectsFromAppConfigJson<HTTPListener>("Listeners/HTTPListeners");
    EXPECT_EQ(1, listeners.size());
    auto processor = PipeLineProcessor::getInstance(test_httplistener::testFilesDir + PROCESS_CONFIG_TEST_FILE_03);
    listeners[0]->init(processor.value());
    listeners[0]->startListening();

    usleep(HALF_A_SECOND);
    std::string stdOut;
    EXPECT_EQ(0, test_httplistener::sendFile(std::string(test_httplistener::testFilesDir + HTTP_REQUEST_TEST_FILE_06), stdOut));
    usleep(HALF_A_SECOND * 4);

    auto processedData = listeners[0]->getLastProcessingData();
    json j = {};
    processedData.value()->toJson(&j);
    EXPECT_EQ("HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 62\r\nContent-Type: text/plain; charset=utf-8\r\nServer: level2 httpListener\r\n\r\n\r\n404 Not Found \nNo processing pipeline matches the request data", stdOut);
    std::cout << std::setw(4) << j << '\n';
}

TEST(HttpResponse, getsInitializedWithDefaults) {
    HttpResponse response;
    EXPECT_EQ("200 OK", response.getStatusCode());
    EXPECT_EQ("HTTP/1.1", response.getProtocol());
    EXPECT_EQ(4, response.getAllHeaderFields()->size());
    EXPECT_EQ("close", response.getHeaderFieldValue("Connection").value_or("failure"));
    EXPECT_EQ("text/plain; charset=utf-8", response.getHeaderFieldValue("Content-Type").value_or("failure"));
    EXPECT_EQ("level2 httpListener", response.getHeaderFieldValue("Server").value_or("failure"));
    EXPECT_EQ("6", response.getHeaderFieldValue("Content-Length").value_or("failure"));
}

TEST(HttpResponse, hasDefaultHeader) {
    HttpResponse response;
    EXPECT_EQ("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 6\r\nContent-Type: text/plain; charset=utf-8\r\nServer: level2 httpListener\r\n\r\n\r\n", response.getHeader());
}

TEST(HttpResponse, headContainsConentLengthIfPayloadWasAdded) {
    HttpResponse response;
    response.setPayload("this is a payload, that contains 45 chars :-)");
    EXPECT_EQ("45", response.getHeaderFieldValue("Content-Length").value_or("failure"));
}

TEST(HttpResponse, settingStatusCodesOtherThan2xxOverwritesThePayload) {
    HttpResponse response;
    EXPECT_EQ("200 OK", std::string(response.getPayloadPointer().value()));
    response.setStatusCode(HTTP_STATUS_CODE_411);
    EXPECT_EQ(HTTP_STATUS_CODE_411, std::string(response.getPayloadPointer().value()));
    response.setStatusCode(HTTP_STATUS_CODE_200);
    EXPECT_EQ(HTTP_STATUS_CODE_411, std::string(response.getPayloadPointer().value()));
}

TEST(HttpResponse, canSetPayloadAsPointerAndPayloadWillBeOwnedByTheHttpResponse) {
    HttpResponse response;
    void* p = malloc(4096);
    memset(p, 42, 4096);
    response.setPayload(p, 4096);
    free(p);
    p = NULL;
    EXPECT_EQ(4096, strlen(response.getPayloadPointer().value()));
    EXPECT_EQ("4096", response.getHeaderFieldValue("Content-Length").value_or("failure"));
}

TEST(HttpResponse, getContentLengthWorks) {
    HttpResponse response;
    void* p = malloc(4096);
    memset(p, 42, 4096);
    response.setPayload(p, 4096);
    free(p);
    p = NULL;
    EXPECT_EQ(4096, response.getContentLength());
}

TEST(HttpResponse, getContentLengthCanHandleInvalidValues) {
    HttpResponse response;
    response.addHeaderField(HTTP_FIELD_NAME_CONTENT_LENGTH, "non numeric value will fail conversion to size_t");
    EXPECT_EQ(0, response.getContentLength());
}
