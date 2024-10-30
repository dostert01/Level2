#ifndef PIPELINE_SELECTABLE_H
#define PIPELINE_SELECTABLE_H

#include <map>
#include <string>
#include <optional>


namespace second_take {

class Matchable {
    public:
        Matchable() = default;
        void addMatchingPattern(std::string key, std::string value);
        uint getCountOfMatchingPatterns();
        std::optional<std::string> getMatchingPattern(std::string key);
        void removeMatchingPattern(std::string key);
    private:
        std::map<std::string, std::string> matchingPatterns;

};

}

#endif //PIPELINE_SELECTABLE_H