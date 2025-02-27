#include <gtest/gtest.h>
#include "../logger/logger.h"
#include "matchable.h"

using namespace level2;

using namespace std;
namespace test_pipeline_payload_matching {
    string workingDir;
    string testFilesDir;

    void configureLogger() {
        Logger& logger = Logger::getInstance();
        logger.setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
        logger.setLoggingDestination(LoggingDestinationFactory().createDestinationStdErr());
    }
}

void configureTest() {
    test_pipeline_payload_matching::configureLogger();
}

TEST(PipeLinePayloadMating, DoesNotMatchIfNumberOfPatternsDiffer) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m1.addMatchingPattern("key01", "value01");
    m2->addMatchingPattern("key01", "value01");
    m2->addMatchingPattern("key02", "value02");
    EXPECT_FALSE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, MatchesIfAllMatches) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key01", "value01");
    m2->addMatchingPattern("key02", "value02");
    EXPECT_TRUE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, DoesNotMatchIfOneKeyDiffers) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key01", "value01");
    m2->addMatchingPattern("key03", "value02");
    EXPECT_FALSE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, DoesNotMatchIfOneValueDiffers) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key01", "value01");
    m2->addMatchingPattern("key02", "value03");
    EXPECT_FALSE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, OrderDoesNotMatter) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key01", "value01");
    EXPECT_TRUE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, CanMatchRegEx) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m1.addMatchingPattern("key01", "^[a-z]{5}[0-9]{2}$");
    m1.addMatchingPattern("key02", "val.*");
    m2->addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key01", "value01");
    EXPECT_TRUE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, ReturnsFalseIfInvalidRegEx) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m1.addMatchingPattern("key01", "^[a-z]{5}[0-9]{2$");
    m2->addMatchingPattern("key01", "value01");
    EXPECT_FALSE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, DoesNotMatchIfBothHaveNoMatchingPatterns) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    EXPECT_FALSE(m1.matchesAll(m2));
}

TEST(PipeLinePayloadMating, MatchesIfAnyOfTheOtherCanBeMatchedToAllOfMine) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m1.addMatchingPattern("key01", "value01");
    m1.addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key01", "value01");
    m2->addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key03", "value03");
    m2->addMatchingPattern("key04", "value04");
    EXPECT_TRUE(m1.matchesAllOfMineToAnyOfTheOther(m2));
}

TEST(PipeLinePayloadMating, MatchesIfTheFirstOneHasNoMatchingPatterns) {
    Matchable m1;
    auto m2 = make_shared<Matchable>();
    m2->addMatchingPattern("key01", "value01");
    m2->addMatchingPattern("key02", "value02");
    m2->addMatchingPattern("key03", "value03");
    m2->addMatchingPattern("key04", "value04");
    EXPECT_TRUE(m1.matchesAllOfMineToAnyOfTheOther(m2));
}

TEST(PipeLinePayloadMating, canCopyContent) {
    Matchable m1;
    Matchable m2;
    
    m1.addMatchingPattern("key05", "value05");

    m2.addMatchingPattern("key01", "value01");
    m2.addMatchingPattern("key02", "value02");
    m2.addMatchingPattern("key03", "value03");
    m2.addMatchingPattern("key04", "value04");

    m1 = m2;
    EXPECT_TRUE(m1.matchesAll(m2));
    EXPECT_EQ(m1.getCountOfMatchingPatterns(), m2.getCountOfMatchingPatterns());
}
