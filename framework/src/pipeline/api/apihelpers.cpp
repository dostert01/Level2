#include "apihelpers.h"
#include <iostream>

std::optional<std::string> getNamedArgument(const PipelineStepInitData& initData, const std::string& argumentName) {
    auto search = initData.namedArguments.find(argumentName);
    if(search != initData.namedArguments.end()) {
        return search->second;
    }
    return std::nullopt;
}
