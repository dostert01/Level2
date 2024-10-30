#include <gtest/gtest.h>
//#include <memory>
//#include <unistd.h>
//#include <iostream>
//#include <regex>
//#include <fstream>
#include "pipeline.h"
#include "../logger/logger.h"

using namespace second_take;

namespace test_pipeline_payload_matching {
    std::string workingDir;
    std::string testFilesDir;

    void configureLogger() {
        Logger& logger = second_take::Logger::getInstance();
        logger.setMaxLogLevel(second_take::LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
    }
}

void configureTest() {
    test_pipeline_payload_matching::configureLogger();
}

TEST(PipeLinePayloadMating, DoesNotMatchIfNumberOfPatternsDiffer) {
    Matchable m1;
    Matchable m2;
    m1.addMatchingPattern("key01", "value01");
    m2.addMatchingPattern("key01", "value01");
    m2.addMatchingPattern("key02", "value02");
    EXPECT_FALSE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, MatchesIfAllMatches) {
    Matchable m1;
    Matchable m2;
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2.addMatchingPattern("key01", "value01");
    m2.addMatchingPattern("key02", "value02");
    EXPECT_TRUE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, DoesNotMatchIfOneKeyDiffers) {
    Matchable m1;
    Matchable m2;
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2.addMatchingPattern("key01", "value01");
    m2.addMatchingPattern("key03", "value02");
    EXPECT_FALSE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, DoesNotMatchIfOneValueDiffers) {
    Matchable m1;
    Matchable m2;
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2.addMatchingPattern("key01", "value01");
    m2.addMatchingPattern("key02", "value03");
    EXPECT_FALSE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, OrderDoesNotMatter) {
    Matchable m1;
    Matchable m2;
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2.addMatchingPattern("key02", "value02");
    m2.addMatchingPattern("key01", "value01");
    EXPECT_TRUE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, CanMatchRegEx) {
    Matchable m1;
    Matchable m2;
    m1.addMatchingPattern("key01", "^[a-z]{5}[0-9]{2}$");
    m1.addMatchingPattern("key02", "val.*");
    m2.addMatchingPattern("key02", "value02");
    m2.addMatchingPattern("key01", "value01");
    EXPECT_TRUE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, ReturnsFalseIfInvalidRegEx) {
    Matchable m1;
    Matchable m2;
    m1.addMatchingPattern("key01", "^[a-z]{5}[0-9]{2$");
    m2.addMatchingPattern("key01", "value01");
    EXPECT_FALSE(m1.matchesAll(m2));
}