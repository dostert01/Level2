#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include <filesystem>
#include <regex>
#include <vector>
#include <application.h>
#include <common.h>

using namespace level2;
using namespace std;
using namespace nlohmann::json_abi_v3_11_3;

#define ENV_VAR_NAME "MY_TEST_VARIABLE"
#define APP_CONFIG_TEST_FILE_06 "/applicationConfig06.json"

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

TEST(ApplicationContext, AlwaysCreatesASingletonInstanceAsPointer) {
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_06);
    EXPECT_NE(nullptr, app.get());
    deleteTestAppDirectory();
}

TEST(ApplicationContext, InitializesTheAppContext)
{
    configureTest();
    std::shared_ptr<Application> app = Application::getInstance(test_application::testFilesDir + APP_CONFIG_TEST_FILE_06);
    EXPECT_NE(nullptr, APP_CONTEXT.getApplicationDirectories());
    deleteTestAppDirectory();
}
