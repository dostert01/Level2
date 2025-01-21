#ifndef PIPELINE_MATCHABLE_H
#define PIPELINE_MATCHABLE_H

#include <map>
#include <string>
#include <optional>
#include <memory>

using namespace std;

namespace event_forge {

class Matchable {
    public:
        Matchable() = default;
        void addMatchingPattern(string key, string value);
        uint getCountOfMatchingPatterns();
        optional<string> getMatchingPattern(string key);
        void setMatchingPatterns(Matchable& other);
        void removeMatchingPattern(string key);
        bool matchesAll(shared_ptr<Matchable> other);
        bool matchesAll(Matchable& other);
        bool matchesAllOfMineToAnyOfTheOther(shared_ptr<Matchable> other);
        bool hasMatchingPatterns();
        map<string, string> getMatchingPatterns();
        Matchable& operator=(const Matchable& other);
    private:
        map<string, string> matchingPatterns;
        bool RegexMatch(string s1, string s2);
        bool bothHaveMatchingPatterns(Matchable& other);
        bool bothHaveSameNumberOfMatchingPatterns(Matchable& other);
        int getCoutOfMatches(shared_ptr<Matchable> other);
        int getCoutOfMatches(Matchable& other);
};

}

#endif //PIPELINE_MATCHABLE_H