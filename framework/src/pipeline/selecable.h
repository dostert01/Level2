#ifndef PIPELINE_SELECTABLE_H
#define PIPELINE_SELECTABLE_H

#include <map>
#include <string>
#include <optional>


namespace second_take {

class Selectable {
    public:
        Selectable() = default;
        void addSelectorPattern(std::string key, std::string value);
        uint getCountOfSelectorPatterns();
        std::optional<std::string> getSelectorPattern(std::string key);
        void removeSelectorPattern(std::string key);
    private:
        std::map<std::string, std::string> selectionPatterns;

};

}

#endif //PIPELINE_SELECTABLE_H