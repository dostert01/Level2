#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include <filesystem>
#include <regex>
#include <vector>
#include "applicationcontext.h"
#include "logger.h"
#include <common.h>

using namespace level2;
using namespace std;
using namespace nlohmann::json_abi_v3_11_3;

#define ENV_VAR_NAME "MY_TEST_VARIABLE"
#define APP_CONFIG_TEST_FILE_01 "/applicationConfig01.json"
#define APP_CONFIG_TEST_FILE_02 "/applicationConfig02.json"
#define APP_CONFIG_TEST_FILE_06 "/applicationConfig06.json"

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

/*
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

TEST(ApplicationContext, CanReadFromEnvironment) {
    setenv(ENV_VAR_NAME, "Hello World", 1);
    optional<string> value = APP_CONTEXT.readEnv(ENV_VAR_NAME);
    unsetenv(ENV_VAR_NAME);
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ("Hello World", value);
}

TEST(ApplicationContext, ReturnsEmptyOptionalIfVariableIsNotDefined) {
    optional<string> value = APP_CONTEXT.readEnv(ENV_VAR_NAME);
    EXPECT_FALSE(value.has_value());
}

TEST(ApplicationContext, CanReadTheCurrentWorkingDirectory) {
    optional<string> value = APP_CONTEXT.getCurrentWorkingDirectory();
    EXPECT_TRUE(value.has_value());
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

TEST(ApplicationContext, canSplitStrings01) {
    vector<string> result = APP_CONTEXT.splitString("Listeners", "/");
    EXPECT_EQ(1, result.size());
    EXPECT_EQ("Listeners", result[0]);
}

TEST(ApplicationContext, canSplitStrings02) {
    vector<string> result = APP_CONTEXT.splitString("Listeners/MQTTListeners", "/");
    for(auto &s : result) {
        LOGGER.info("split result: " + s);
    }
    EXPECT_EQ(2, result.size());
    EXPECT_EQ("Listeners", result[0]);
    EXPECT_EQ("MQTTListeners", result[1]);
}

TEST(ApplicationContext, canSplitStrings03) {
    vector<string> result = APP_CONTEXT.splitString("Listeners/MQTTListeners/AndMuchMore", "/");
    for(auto &s : result) {
        LOGGER.info("split result: " + s);
    }
    EXPECT_EQ(3, result.size());
    EXPECT_EQ("Listeners", result[0]);
    EXPECT_EQ("MQTTListeners", result[1]);
    EXPECT_EQ("AndMuchMore", result[2]);
}

TEST(ApplicationContext, canSplitStrings04) {
    vector<string> result = APP_CONTEXT.splitString("Listeners/MQTTListeners/And/Much/More", "/");
    for(auto &s : result) {
        LOGGER.info("split result: " + s);
    }
    EXPECT_EQ(5, result.size());
    EXPECT_EQ("Listeners", result[0]);
    EXPECT_EQ("MQTTListeners", result[1]);
    EXPECT_EQ("And", result[2]);
    EXPECT_EQ("Much", result[3]);
    EXPECT_EQ("More", result[4]);
}
*/
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
/*
TEST(ApplicationContext, CreatesAnEmptyVectorIfConfigIsNotFound) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MockListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MockListener>("Listeners/Not/To/Be/Found/In/Json");
    EXPECT_EQ(0, listeners.size());
}

TEST(ApplicationContext, CanCreateAnObjectOfTypeMockListnerFromAppConfig) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_01);
    vector<shared_ptr<MockListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MockListener>("Listeners/MQTTListeners");
    EXPECT_EQ(2, listeners.size());
}
*/
TEST(ApplicationContext, FailureInParsingLeadsToMissingObject) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_02);
    vector<shared_ptr<MockListener>> listeners = APP_CONTEXT.createObjectsFromAppConfigJson<MockListener>("Listeners/MQTTListeners");
    EXPECT_EQ(1, listeners.size());
}


TEST(ApplicationContext, canReadApplicationRootDirAndExpandEnvVariables) {
    common::getCurrentWorkingDirectory();
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_applicationcontext::testFilesDir + APP_CONFIG_TEST_FILE_06);
    vector<shared_ptr<ApplicationDirectories>> dirs = APP_CONTEXT.createObjectsFromAppConfigJson<ApplicationDirectories>("applicationDirectories");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ(common::getCurrentWorkingDirectory().value_or("") + "/opt/testApplication", dirs[0]->getApplicationRootDir());
}
