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
#define APP_CONFIG_TEST_FILE_06 "/applicationConfig06.json"
#define APP_CONFIG_TEST_FILE_07 "/applicationConfig07.json"
#define APP_CONFIG_TEST_FILE_09 "/applicationConfig09.json"
#define APP_CONFIG_TEST_FILE_10 "/applicationConfig10.json"
#define APP_CONFIG_TEST_FILE_11 "/applicationConfig11.json"
#define APP_CONFIG_TEST_FILE_12 "/applicationConfig12.json"
#define APP_CONFIG_TEST_FILE_13 "/applicationConfig13.json"
#define PROCESS_CONFIG_TEST_FILE_01 "/processConfig01.json"
#define PROCESS_CONFIG_TEST_FILE_03 "/processConfig03.json"
#define PIPELINE_CONFIG_TEST_FILE_01 "/pipelineConfig01.json"
#define PIPELINE_CONFIG_TEST_FILE_03 "/pipelineConfig03.json"
#define PIPELINE_CONFIG_TEST_FILE_04 "/pipelineConfig04.json"
#define PIPELINE_CONFIG_TEST_FILE_08 "/pipelineConfig08.json"

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
    APP_CONTEXT.resetAllMemberVariables();
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

TEST(ApplicationDirectories, canReadApplicationRootDirAndExpandEnvVariables) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_application::testFilesDir + APP_CONFIG_TEST_FILE_06);
    auto app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_06);
    auto dirs = app->getApplicationDirectories();
    EXPECT_EQ(StaticStringFunctions::readEnv("PWD").value_or("") + "/opt/testApplication", dirs->getApplicationRootDir());
    deleteTestAppDirectory();
}

TEST(ApplicationDirectories, pathsGetPrefixedWithApplicationRootPath) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_application::testFilesDir + APP_CONFIG_TEST_FILE_06);
    auto app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_06);
    auto dirs = app->getApplicationDirectories();
    std::string cwd = StaticStringFunctions::readEnv("PWD").value_or("");
    EXPECT_EQ(cwd + "/opt/testApplication/etc/pipelines.d", dirs->getPipelinesDir());
    EXPECT_EQ(cwd + "/opt/testApplication/etc/processes.d", dirs->getProcessesDir());
    EXPECT_EQ(cwd + "/opt/testApplication/lib", dirs->getWorkerModulesDir());
    deleteTestAppDirectory();
}

TEST(ApplicationDirectories, canHandleInvalidInputByFallingBackToDefaults) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_application::testFilesDir + APP_CONFIG_TEST_FILE_07);
    auto app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_07);
    auto dirs = app->getApplicationDirectories();
    std::string appRoot = StaticStringFunctions::getCurrentWorkingDirectory().value_or("");
    EXPECT_EQ(appRoot + "/pipelines.d", dirs->getPipelinesDir());
    EXPECT_EQ(appRoot + "/processes.d", dirs->getProcessesDir());
    EXPECT_EQ(appRoot + "/lib", dirs->getWorkerModulesDir());
    deleteTestAppDirectory();
}

TEST(ApplicationDirectories, canHandleMissingInputByFallingBackToDefaults) {
    configureTest();
    APP_CONTEXT.loadApplicationConfig(test_application::testFilesDir + APP_CONFIG_TEST_FILE_09);
    auto app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_09);
    auto dirs = app->getApplicationDirectories();
    std::string appRoot = StaticStringFunctions::getCurrentWorkingDirectory().value_or("");
    EXPECT_EQ(appRoot + "/pipelines.d", dirs->getPipelinesDir());
    EXPECT_EQ(appRoot + "/processes.d", dirs->getProcessesDir());
    EXPECT_EQ(appRoot + "/lib", dirs->getWorkerModulesDir());
    deleteTestAppDirectory();
}

TEST(ApplicationDirectories, canEnsureThatDirectoriesExist) {
    configureTest();
    filesystem::remove_all("./opt");
    APP_CONTEXT.loadApplicationConfig(test_application::testFilesDir + APP_CONFIG_TEST_FILE_06);
    auto app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_06);
    auto dirs = app->getApplicationDirectories();
    std::string cwd = StaticStringFunctions::readEnv("PWD").value_or("");
    EXPECT_TRUE(filesystem::exists(cwd + "/opt/testApplication/etc/processes.d"));
    filesystem::remove_all("./opt");
    deleteTestAppDirectory();
}

TEST(Application, InitializesTheAppContext) {
    configureTest();
    EXPECT_EQ(nullptr, APP_CONTEXT.getApplicationDirectories());
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_10);
    EXPECT_NE(nullptr, APP_CONTEXT.getApplicationDirectories());
    deleteTestAppDirectory();
}

TEST(Application, canInitializeTheListeners) {
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_10);
    vector<shared_ptr<Listener>> listeners = app->getListeners();
    EXPECT_EQ(2, listeners.size());
    deleteTestAppDirectory();
}

TEST(Application, canInitializeAllTypesOfListeners) {
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

TEST(Application, listenersWithoutNamePropertyAreSkipped) {
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_12);
    vector<shared_ptr<Listener>> listeners = app->getListeners();
    EXPECT_EQ(3, listeners.size());
    EXPECT_EQ("httpGetListener02", listeners[0]->getName());
    EXPECT_EQ("fileListener02", listeners[1]->getName());
    EXPECT_EQ("mqttListener01", listeners[2]->getName());
    deleteTestAppDirectory();
}

TEST(Application, loadsAllProcessesFromProcessesDirectory) {
    configureTest();


    auto app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_11);
    std::string processesDir = app->getApplicationDirectories()->getProcessesDir();
    std::string pipelinesDir = app->getApplicationDirectories()->getPipelinesDir();
    std::filesystem::copy_file(test_application::testFilesDir + PROCESS_CONFIG_TEST_FILE_01, processesDir + PROCESS_CONFIG_TEST_FILE_01);
    std::filesystem::copy_file(test_application::testFilesDir + PROCESS_CONFIG_TEST_FILE_03, processesDir + PROCESS_CONFIG_TEST_FILE_03);
    std::filesystem::copy_file(test_application::testFilesDir + PIPELINE_CONFIG_TEST_FILE_01, pipelinesDir + PIPELINE_CONFIG_TEST_FILE_01);
    std::filesystem::copy_file(test_application::testFilesDir + PIPELINE_CONFIG_TEST_FILE_03, pipelinesDir + PIPELINE_CONFIG_TEST_FILE_03);
    std::filesystem::copy_file(test_application::testFilesDir + PIPELINE_CONFIG_TEST_FILE_04, pipelinesDir + PIPELINE_CONFIG_TEST_FILE_04);
    std::filesystem::copy_file(test_application::testFilesDir + PIPELINE_CONFIG_TEST_FILE_08, pipelinesDir + PIPELINE_CONFIG_TEST_FILE_08);
    app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_11);
    EXPECT_EQ(2, app->getProcesses().size());
    deleteTestAppDirectory();
}
