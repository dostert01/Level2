#ifndef API_HELPERS_H
#define  API_HELPERS_H

#include <map>
#include <string>
#include <memory>

struct PipelineStepInitData
{
    std::multimap<std::string, std::string> namedArguments;
};

struct PipelineProcessingData
{
    mutable std::multimap<std::string, std::unique_ptr<std::string>> namedPayloadData;
};

std::optional<std::string> getNamedArgument(const PipelineStepInitData& initData, const std::string& argumentName);

#endif //#ifndef API_HELPERS_H