#ifndef API_HELPERS_H
#define  API_HELPERS_H

#include <map>
#include <string>
#include <memory>

struct LibInitData
{
    std::map<std::string, std::string> namedArguments;
    std::string s;
};

struct LibProcessingData
{
    std::map<std::string, std::unique_ptr<std::string>> namedPayloadData;
};

std::optional<std::string> getNamedArgument(LibInitData* initData, const std::string& argumentName);

#endif //#ifndef API_HELPERS_H