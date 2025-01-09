#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include "applicationcontext.h"
#include "logger.h"

using namespace event_forge;
using namespace std;

#define ENV_VAR_NAME "MY_TEST_VARIABLE"


namespace test_applicationcontext {
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

TEST(ApplicationContext, CanLoadApplicationConfigFromFile) {
    APP_CONTEXT.loadApplicationConfig();
    EXPECT_TRUE(value.has_value());
}
