#ifndef API_HELPERS_H
#define  API_HELPERS_H

#include <map>
#include <string>
#include <memory>
#include <vector>

struct PipelineStepInitData
{
    std::multimap<std::string, std::string> namedArguments;
};

struct ProcessingPayload {
    std::string mimetype;
    std::unique_ptr<std::vector<std::uint8_t>> payloadData;
    ProcessingPayload() = default;
    std::string payloadAsString();
};

struct PipelineProcessingData
{
    std::multimap<std::string, ProcessingPayload*> namedPayloadData;
    PipelineProcessingData() = default;
    ~PipelineProcessingData();
    void addPayloadData(std::string payloadName, std::string mimetype, std::string data);
    std::optional<ProcessingPayload*> getPayload(std::string payloadName);
};

std::optional<std::string> getNamedArgument(const PipelineStepInitData& initData, const std::string& argumentName);

#endif //#ifndef API_HELPERS_H