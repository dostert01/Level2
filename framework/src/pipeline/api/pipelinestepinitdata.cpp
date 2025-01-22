#include "pipelinestepinitdata.h"

namespace event_forge {

std::optional<std::string> PipelineStepInitData::getNamedArgument(const std::string& argumentName) {
    auto search = namedArguments.find(argumentName);
    if(search != namedArguments.end()) {
        return search->second;
    }
    return std::nullopt;
}

void PipelineStepInitData::addNamedArgument(std::string name, std::string value) {
    namedArguments.insert({name, value});
}

}
