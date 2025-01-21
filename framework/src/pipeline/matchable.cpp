#include <regex>
#include <iostream>

#include "matchable.h"
#include "../logger/logger.h"

namespace event_forge {

Matchable& Matchable::operator=(const Matchable& other) {
    matchingPatterns.clear();
    matchingPatterns = ((Matchable)other).getMatchingPatterns();
    return *this;
}

void Matchable::addMatchingPattern(string key, string value) {
    matchingPatterns[key] = value;
}

uint Matchable::getCountOfMatchingPatterns() {
    return matchingPatterns.size();
}

optional<string> Matchable::getMatchingPattern(string key) {
    if(matchingPatterns.contains(key)) {
        return matchingPatterns[key];
    } else {
        return nullopt;
    }
}

void Matchable::setMatchingPatterns(Matchable& other) {
    *this = other;
}

void Matchable::removeMatchingPattern(string key) {
    matchingPatterns.erase(key);
}

int Matchable::getCoutOfMatches(Matchable& other) {
    int matchCount = 0;
    for(const auto& [key, value] : matchingPatterns) {
        optional<string> foundMatchingPattern = other.getMatchingPattern(key);
        if(foundMatchingPattern.has_value() && RegexMatch(value, foundMatchingPattern.value())) {
            matchCount++;
        }
    }
    return matchCount;
}

int Matchable::getCoutOfMatches(shared_ptr<Matchable> other) {
    return getCoutOfMatches(*other);
}

bool Matchable::matchesAllOfMineToAnyOfTheOther(shared_ptr<Matchable> other) {
    return (getCountOfMatchingPatterns() == 0) ||
        (getCountOfMatchingPatterns() == getCoutOfMatches(other));
}

bool Matchable::matchesAll(Matchable& other) {
    int matchCount = 0;
    if(bothHaveMatchingPatterns(other) && bothHaveSameNumberOfMatchingPatterns(other)) {
        matchCount = getCoutOfMatches(other);
    } else {
        LOGGER.info("Patterns do not match because of different counts");
    }
    LOGGER.info("Patterns matched " + to_string(matchCount) + "/" + to_string(getCountOfMatchingPatterns()));
    return (matchCount == getCountOfMatchingPatterns()) && (matchCount > 0);

}

bool Matchable::matchesAll(shared_ptr<Matchable> other) {
    return matchesAll(*other);
}

bool Matchable::bothHaveMatchingPatterns(Matchable& other) {
    return (this->getCountOfMatchingPatterns() > 0) && (other.getCountOfMatchingPatterns() > 0);
}

bool Matchable::bothHaveSameNumberOfMatchingPatterns(Matchable& other) {
    return this->getCountOfMatchingPatterns() == other.getCountOfMatchingPatterns();
}

bool Matchable::RegexMatch(string s1, string s2) {
    bool match = false;
    try {
        regex regex(s1);
        match = regex_match(s2, regex);
    } catch (const exception& e) {
        LOGGER.error("Failed to match regular expression '" + s1 + "' with value '" + s2 + "' - " + e.what());
    }
    return match;
}

bool Matchable::hasMatchingPatterns() {
    return getCountOfMatchingPatterns() > 0;
}

map<string, string> Matchable::getMatchingPatterns() {
    return matchingPatterns;
}

}