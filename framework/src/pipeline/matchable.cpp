#include "matchable.h"

namespace second_take {

void Matchable::addMatchingPattern(std::string key, std::string value) {
    matchingPatterns[key] = value;
}

uint Matchable::getCountOfMatchingPatterns() {
    return matchingPatterns.size();
}

std::optional<std::string> Matchable::getMatchingPattern(std::string key) {
    if(matchingPatterns.find(key) != matchingPatterns.end()) {
        return matchingPatterns[key];
    } else {
        return std::nullopt;
    }
}

void Matchable::removeMatchingPattern(std::string key) {
    matchingPatterns.erase(key);
}

}