#ifndef PIPELINE_MATCHABLE_H
#define PIPELINE_MATCHABLE_H

#include <map>
#include <string>
#include <optional>


namespace event_forge {

class Matchable {
    public:
        Matchable() = default;
        void addMatchingPattern(std::string key, std::string value);
        uint getCountOfMatchingPatterns();
        std::optional<std::string> getMatchingPattern(std::string key);
        void removeMatchingPattern(std::string key);
        bool matchesAll(Matchable& other);
        bool hasMatchingPatterns();
    private:
        std::map<std::string, std::string> matchingPatterns;
        bool RegexMatch(std::string s1, std::string s2);
        bool bothHaveMatchingPatterns(Matchable& other);
        bool bothHaveSameNumberOfMatchingPatterns(Matchable& other);
};

}

#endif //PIPELINE_MATCHABLE_H