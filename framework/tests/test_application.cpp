#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include <filesystem>
#include <regex>
#include <vector>

#include "application.h"
#include "common.h"
#include "listeners.h"

using namespace level2;
using namespace std;
using namespace nlohmann::json_abi_v3_11_3;

#define ENV_VAR_NAME "MY_TEST_VARIABLE"
#define APP_CONFIG_TEST_FILE_10 "/applicationConfig10.json"
#define APP_CONFIG_TEST_FILE_11 "/applicationConfig11.json"
#define APP_CONFIG_TEST_FILE_12 "/applicationConfig12.json"

namespace test_application {
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
    test_application::configureLogger();
    test_application::configureTestVariables();
}

void deleteTestAppDirectory() {
    std::string cwd = StaticStringFunctions::readEnv("PWD").value_or("");
    filesystem::remove_all(cwd + "/opt");
}

TEST(ApplicationContext, canCreateAnInstance) {
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_10);
    EXPECT_NE(nullptr, app.get());
    deleteTestAppDirectory();
}

TEST(ApplicationContext, InitializesTheAppContext) {
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_10);
    EXPECT_NE(nullptr, APP_CONTEXT.getApplicationDirectories());
    deleteTestAppDirectory();
}

TEST(ApplicationContext, canInitializeTheListeners) {
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_10);
    vector<shared_ptr<Listener>> listeners = app->getListeners();
    EXPECT_EQ(2, listeners.size());
    deleteTestAppDirectory();
}

TEST(ApplicationContext, canInitializeAllTypesOfListeners) {
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_11);
    vector<shared_ptr<Listener>> listeners = app->getListeners();
    EXPECT_EQ(6, listeners.size());
    EXPECT_EQ("httpGetListener01", listeners[0]->getName());
    EXPECT_EQ("httpGetListener02", listeners[1]->getName());
    EXPECT_EQ("fileListener01", listeners[2]->getName());
    EXPECT_EQ("fileListener02", listeners[3]->getName());
    EXPECT_EQ("mqttListener01", listeners[4]->getName());
    EXPECT_EQ("mqttListener02", listeners[5]->getName());
    deleteTestAppDirectory();
}

TEST(ApplicationContext, listenersWithoutNamePropertyAreSkipped) {
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_12);
    vector<shared_ptr<Listener>> listeners = app->getListeners();
    EXPECT_EQ(3, listeners.size());
    EXPECT_EQ("httpGetListener02", listeners[0]->getName());
    EXPECT_EQ("fileListener02", listeners[1]->getName());
    EXPECT_EQ("mqttListener01", listeners[2]->getName());
    deleteTestAppDirectory();
}
