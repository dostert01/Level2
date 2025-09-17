#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include <filesystem>
#include <regex>
#include <vector>
#include "applicationcontext.h"
#include "logger.h"
#include "common.h"

using namespace level2;
using namespace std;
using namespace nlohmann::json_abi_v3_11_3;

#define ENV_VAR_NAME "MY_TEST_VARIABLE"
#define APP_CONFIG_TEST_FILE_01 "/applicationConfig01.json"
#define APP_CONFIG_TEST_FILE_02 "/applicationConfig02.json"
#define APP_CONFIG_TEST_FILE_06 "/applicationConfig06.json"
#define APP_CONFIG_TEST_FILE_07 "/applicationConfig07.json"
#define APP_CONFIG_TEST_FILE_08 "/applicationConfig08.json"

namespace test_applicationcontext {
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

}

void configureTest() {
    test_applicationcontext::configureLogger();
    test_applicationcontext::configureTestVariables();
}

TEST(ApplicationContext, AlwaysCreatesASingletonInstanceAsPointer) {
    ApplicationContext* ctx1 = ApplicationContext::getInstanceAsPointer();
    ApplicationContext* ctx2 = ApplicationContext::getInstanceAsPointer();
    EXPECT_EQ(ctx1, ctx2);
}

TEST(ApplicationContext, AlwaysCreatesASingletonInstanceAsReference) {
    ApplicationContext& ctx1 = ApplicationContext::getInstance();
    ApplicationContext& ctx2 = ApplicationContext::getInstance();
    EXPECT_EQ(ctx1, ctx2);
}

TEST(ApplicationContext, LogsFailureIfParsingJsonFails) {
    testing::internal::CaptureStdout();
    APP_CONTEXT.loadApplicationConfig("");
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("parse error at line 1, column 1: attempting to parse an empty input");
    EXPECT_TRUE(regex_search(output, regex));
}

TEST(ApplicationContext, CanLoadAppConfigFromJsonFile) {
    configureTest();
    testing::internal::CaptureStdout();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_01);
    string output = testing::internal::GetCapturedStdout();
    cout << "output: " << output << endl;
    regex regex("parse error at line 1, column 1: attempting to parse an empty input");
    EXPECT_FALSE(regex_search(output, regex));
}

class MockListener {
    public:
        MockListener() = default;
        MockListener(json jsonObject){
            hostName = jsonObject["hostName"];
            port = jsonObject["port"];
            clientId = jsonObject["clientId"];
        };
    public:
        string hostName = "none";
        int port = 1234;
        string clientId = "hallo";
};

TEST(ApplicationContext, CreatesAnEmptyVectorIfConfigIsNotFound) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MockListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MockListener>("Listeners/Not/To/Be/Found/In/Json");
    EXPECT_EQ(0, listeners.size());
}

TEST(ApplicationContext, CanCreateAnObjectOfTypeMockListenerFromAppConfig) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MockListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MockListener>("Listeners/MQTTListeners");
    EXPECT_EQ(2, listeners.size());
}

TEST(ApplicationContext, FailureInParsingLeadsToUsingTheDefaultConstructor) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_02);
    vector<shared_ptr<MockListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MockListener>("Listeners/MQTTListeners");
    EXPECT_EQ(2, listeners.size());
    EXPECT_EQ(1234, listeners[0]->port);
    EXPECT_EQ(1883, listeners[1]->port);
}

TEST(ApplicationDirectories, canReadApplicationRootDirAndExpandEnvVariables) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_06);
    vector<shared_ptr<ApplicationDirectories>> dirs = APP_CONTEXT.createObjectsFromAppConfigJson<ApplicationDirectories>("applicationDirectories");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ(StaticStringFunctions::readEnv("PWD").value_or("") + "/opt/testApplication", dirs[0]->getApplicationRootDir());
}

TEST(ApplicationDirectories, pathsGetPrefixedWithApplicationRootPath) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_06);
    vector<shared_ptr<ApplicationDirectories>> dirs = APP_CONTEXT.createObjectsFromAppConfigJson<ApplicationDirectories>("applicationDirectories");
    std::string cwd = StaticStringFunctions::readEnv("PWD").value_or("");
    EXPECT_EQ(cwd + "/opt/testApplication/etc/pipelines.d", dirs[0]->getPipelinesDir());
    EXPECT_EQ(cwd + "/opt/testApplication/etc/processes.d", dirs[0]->getProcessesDir());
    EXPECT_EQ(cwd + "/opt/testApplication/lib", dirs[0]->getWorkerModulesDir());
}

TEST(ApplicationDirectories, canHandleInvalidInputByFallingBackToDefaults) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_07);
    vector<shared_ptr<ApplicationDirectories>> dirs = APP_CONTEXT.createObjectsFromAppConfigJson<ApplicationDirectories>("applicationDirectories");
    std::string appRoot = StaticStringFunctions::getCurrentWorkingDirectory().value_or("");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ(appRoot + "/pipelines.d", dirs[0]->getPipelinesDir());
    EXPECT_EQ(appRoot + "/processes.d", dirs[0]->getProcessesDir());
    EXPECT_EQ(appRoot + "/lib", dirs[0]->getWorkerModulesDir());
}

TEST(ApplicationDirectories, canEnsureThatDirectoriesExist) {
    configureTest();
    filesystem::remove_all("./opt");
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_06);
    vector<shared_ptr<ApplicationDirectories>> dirs = APP_CONTEXT.createObjectsFromAppConfigJson<ApplicationDirectories>("applicationDirectories");
    std::string cwd = StaticStringFunctions::readEnv("PWD").value_or("");
    EXPECT_TRUE(dirs[0]->createApplicationDirectories());
    EXPECT_TRUE(filesystem::exists(cwd + "/opt/testApplication/etc/processes.d"));
    filesystem::remove_all("./opt");
}

TEST(ApplicationContext, canConfigureLogger) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_06);
    LOGGER.removeAllDestinations();
    APP_CONTEXT.configureLogger();
}

TEST(ApplicationContext, canConfigureLoggingDestinations) {
    configureTest();
    EXPECT_EQ(1, LOGGER.getCountOfLoggingDestinations());
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_06);
    EXPECT_EQ(4, LOGGER.getCountOfLoggingDestinations());
}

TEST(ApplicationContext, createsAtLeastOneLoggingDestination) {
    configureTest();
    EXPECT_EQ(1, LOGGER.getCountOfLoggingDestinations());
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_02);
    EXPECT_EQ(1, LOGGER.getCountOfLoggingDestinations());
}

TEST(ApplicationContext, canSetLogLevel) {
    configureTest();
    LOGGER.setMaxLogLevel(LogLevel::LOG_LEVEL_NO_LOGGING);
    EXPECT_EQ(LogLevel::LOG_LEVEL_NO_LOGGING, LOGGER.getLogLevel());
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_06);
    EXPECT_EQ(LogLevel::LOG_LEVEL_DEBUG, LOGGER.getLogLevel());
}

TEST(ApplicationContext, doesNotChangeLogLevelOnUnknownParameterValue) {
    configureTest();
    LOGGER.setMaxLogLevel(LogLevel::LOG_LEVEL_NO_LOGGING);
    EXPECT_EQ(LogLevel::LOG_LEVEL_NO_LOGGING, LOGGER.getLogLevel());
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_08);
    EXPECT_EQ(LogLevel::LOG_LEVEL_NO_LOGGING, LOGGER.getLogLevel());
}

TEST(ApplicationContextAutoInit, loadApplicationConfigConfiguresTheLogger) {
    configureTest();
    LOGGER.setMaxLogLevel(LogLevel::LOG_LEVEL_NO_LOGGING);
    EXPECT_EQ(LogLevel::LOG_LEVEL_NO_LOGGING, LOGGER.getLogLevel());
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_06);
    EXPECT_EQ(LogLevel::LOG_LEVEL_DEBUG, LOGGER.getLogLevel());
}
