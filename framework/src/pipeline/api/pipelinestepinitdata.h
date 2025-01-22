#pragma once

#include <map>
#include <string>

namespace event_forge {

class PipelineStepInitData
{
    public:
        PipelineStepInitData() = default;
        std::optional<std::string> getNamedArgument(const std::string& argumentName);
        void addNamedArgument(std::string name, std::string value);
    private:
        std::multimap<std::string, std::string> namedArguments;
};

}
