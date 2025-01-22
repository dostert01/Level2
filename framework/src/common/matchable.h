#ifndef PIPELINE_MATCHABLE_H
#define PIPELINE_MATCHABLE_H

#include <map>
#include <string>
#include <optional>
#include <memory>

namespace event_forge {

class Matchable {
    public:
        Matchable() = default;
        void addMatchingPattern(std::string key, std::string value);
        uint getCountOfMatchingPatterns();
        std::optional<std::string> getMatchingPattern(std::string key);
        void setMatchingPatterns(Matchable& other);
        void removeMatchingPattern(std::string key);
        bool matchesAll(std::shared_ptr<Matchable> other);
        bool matchesAll(Matchable& other);
        bool matchesAllOfMineToAnyOfTheOther(std::shared_ptr<Matchable> other);
        bool hasMatchingPatterns();
        std::map<std::string, std::string> getMatchingPatterns();
        Matchable& operator=(const Matchable& other);
    private:
        std::map<std::string, std::string> matchingPatterns;
        bool RegexMatch(std::string s1, std::string s2);
        bool bothHaveMatchingPatterns(Matchable& other);
        bool bothHaveSameNumberOfMatchingPatterns(Matchable& other);
        int getCoutOfMatches(std::shared_ptr<Matchable> other);
        int getCoutOfMatches(Matchable& other);
};

}

#endif //PIPELINE_MATCHABLE_H