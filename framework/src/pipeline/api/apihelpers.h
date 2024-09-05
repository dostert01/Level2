#ifndef API_HELPERS_H
#define  API_HELPERS_H

#include <map>
#include <string>
#include <memory>

struct LibInitData
{
    std::multimap<std::string, std::string> namedArguments;
};

struct LibProcessingData
{
    std::multimap<std::string, std::unique_ptr<std::string>> namedPayloadData;
};

std::optional<std::string> getNamedArgument(LibInitData& initData, const std::string& argumentName);

#endif //#ifndef API_HELPERS_H