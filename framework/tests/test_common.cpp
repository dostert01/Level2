#include <gtest/gtest.h>
#include <filesystem>

#include "../logger/logger.h"
#include "common.h"

using namespace level2;
using namespace std;

#define LOREM_IPSUM "Lorem ipsum dolor sit amet, consetetur sadipscing elitr,\nsed diam nonumy eirmod tempor invidunt \n"

namespace test_common {
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
    test_common::configureLogger();
    test_common::configureTestVariables();
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
