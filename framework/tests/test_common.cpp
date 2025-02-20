#include <gtest/gtest.h>
#include <filesystem>

#include "../logger/logger.h"
#include "common.h"

using namespace level2;
using namespace std;

#define LOREM_IPSUM "Lorem ipsum dolor sit amet, consetetur sadipscing elitr,\nsed diam nonumy eirmod tempor invidunt \n"

namespace test_StaticStringFunctions {
    string workingDir;
    string testFilesDir;

    void configureLogger() {
        Logger& logger = Logger::getInstance();
        logger.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
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
    test_StaticStringFunctions::configureLogger();
    test_StaticStringFunctions::configureTestVariables();
}

TEST(Base64, canEncodeShortString) {
    EXPECT_EQ("SGFsbG8gV2VsdA==", level2::Base64Encoder::encode("Hallo Welt", 76));
}

TEST(Base64, canEncodeLongStringWithOutNewLine) {
    EXPECT_EQ("TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNldGV0dXIgc2FkaXBzY2luZyBlbGl0ciwKc2VkIGRpYW0gbm9udW15IGVpcm1vZCB0ZW1wb3IgaW52aWR1bnQgCg==",
        level2::Base64Encoder::encodeNoNewLines(
            LOREM_IPSUM));
}

TEST(Base64, canEncodeLongStringWithNewLine) {
    std::string result = level2::Base64Encoder::encodeWithDefaultNewLines(LOREM_IPSUM);
    EXPECT_EQ("TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNldGV0dXIgc2FkaXBzY2luZyBlbGl0ciwK\nc2VkIGRpYW0gbm9udW15IGVpcm1vZCB0ZW1wb3IgaW52aWR1bnQgCg==",
        result);
    EXPECT_EQ("\n", result.substr(76,1));
}

TEST(systemUtils, canReadExistingEnvVariable) {
    std::string envVarName = "systemUtils_canReadExistingEnvVariable";
    setenv(envVarName.c_str(), "hallo", 1);
    auto result = level2::StaticStringFunctions::readEnv(envVarName);
    EXPECT_EQ("hallo", result.value_or(""));
    unsetenv(envVarName.c_str());
}

TEST(systemUtils, readEnvReturnsEmptyOptionalIfVariableDoesNotExists) {
    auto result = level2::StaticStringFunctions::readEnv("nonExistingEnvVar_readEnvReturnsEmptyOptionalIfVariableDoesNotExists");
    EXPECT_FALSE(result.has_value());
}

TEST(systemUtils, splitStringCanSplitAPath01) {
    auto result = level2::StaticStringFunctions::splitString("this/is/a/path", "/");
    EXPECT_EQ(4, result.size());
}

TEST(systemUtils, splitStringCanSplitAPath02) {
    auto result = level2::StaticStringFunctions::splitString("/this/is/a/path", "/");
    EXPECT_EQ(4, result.size());
    EXPECT_EQ("this", result[0]);
}

TEST(systemUtils, splitStringCanSplitAPath03) {
    auto result = level2::StaticStringFunctions::splitString("this/is/a/path/", "/");
    EXPECT_EQ(4, result.size());
    EXPECT_EQ("path", result[3]);
}

TEST(systemUtils, splitStringCanSplitAPath04) {
    auto result = level2::StaticStringFunctions::splitString("hello", "/");
    EXPECT_EQ(1, result.size());
    EXPECT_EQ("hello", result[0]);
}

TEST(systemUtils, splitStringCanUseLongSeparators) {
    auto result = level2::StaticStringFunctions::splitString("###this###is###a###path###", "###");
    EXPECT_EQ(4, result.size());
    EXPECT_EQ("path", result[3]);
}

TEST(systemUtils, replaceEnvVarsInPathWorksWithJustAnEnvVar) {
    std::string envVarName = "systemUtils_replaceEnvVarsInPathWorksWithJustAnEnvVar";
    setenv(envVarName.c_str(), "hallo", 1);
    auto result = level2::StaticStringFunctions::replaceEnvVariablesInPath("$" + envVarName);
    EXPECT_EQ("hallo", result);
    unsetenv(envVarName.c_str());
}

TEST(systemUtils, doesNotTouchNotExistingEnvVars) {
    auto result = level2::StaticStringFunctions::replaceEnvVariablesInPath("$nonExistingEnvVar_systemUtils");
    EXPECT_EQ("$nonExistingEnvVar_systemUtils", result);
}

TEST(systemUtils, replaceEnvVarsInPathWorks) {
    std::string envVarName = "systemUtils_replaceEnvVarsInPathWorks";
    setenv(envVarName.c_str(), "hallo", 1);
    auto result = level2::StaticStringFunctions::replaceEnvVariablesInPath("/start/$" + envVarName + "/end");
    EXPECT_EQ("start/hallo/end", result);
    unsetenv(envVarName.c_str());
}

TEST(staticFileFunctions, canCreateDirectory) {
    std::string basedirectoryName = "./staticFileFunctions_test";
    std::string directoryName = basedirectoryName + "/this/is/a/test/directory";
    filesystem::remove_all(basedirectoryName);
    EXPECT_FALSE(filesystem::exists(directoryName));
    auto[success, message] = StaticFileFunctions::createDirectory(directoryName);
    EXPECT_TRUE(success);
    EXPECT_EQ("", message);
    EXPECT_TRUE(filesystem::exists(directoryName));
    filesystem::remove_all(basedirectoryName);
}

TEST(staticFileFunctions, createDirectoryRetrunsTrueIfDirectoryAlreadyExists) {
    std::string basedirectoryName = "./staticFileFunctions_test";
    std::string directoryName = basedirectoryName + "/this/is/a/test/directory";
    filesystem::remove_all(basedirectoryName);
    StaticFileFunctions::createDirectory(directoryName);
    auto[success, message] = StaticFileFunctions::createDirectory(directoryName);
    EXPECT_TRUE(success);
    EXPECT_EQ("", message);
    EXPECT_TRUE(filesystem::exists(directoryName));
    filesystem::remove_all(basedirectoryName);
}

TEST(staticFileFunctions, createDirectoryRetrunsFalseIfDirectoryCanNotBeCreated) {
    std::string basedirectoryName = "/thisDirectoryShouldNotBeCreatable";
    std::string directoryName = basedirectoryName + "/this/is/a/test/directory";
    auto[success, message] = StaticFileFunctions::createDirectory(directoryName);
    EXPECT_FALSE(success);
    EXPECT_TRUE(message.find("success: false - exception has been thrown") != std::string::npos);
    EXPECT_FALSE(filesystem::exists(directoryName));
    filesystem::remove_all(basedirectoryName);
}
