#include "selecable.h"

namespace second_take {

void Selectable::addSelectorPattern(std::string key, std::string value) {
    selectionPatterns[key] = value;
}

uint Selectable::getCountOfSelectorPatterns() {
    return selectionPatterns.size();
}

std::optional<std::string> Selectable::getSelectorPattern(std::string key) {
    if(selectionPatterns.find(key) != selectionPatterns.end()) {
        return selectionPatterns[key];
    } else {
        return std::nullopt;
    }
}

void Selectable::removeSelectorPattern(std::string key) {
    selectionPatterns.erase(key);
}

}