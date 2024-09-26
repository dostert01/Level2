#include "apihelpers.h"
#include <iostream>

std::optional<std::string> getNamedArgument(const PipelineStepInitData& initData, const std::string& argumentName) {
    auto search = initData.namedArguments.find(argumentName);
    if(search != initData.namedArguments.end()) {
        return search->second;
    }
    return std::nullopt;
}

void PipelineProcessingData::addPayloadData(std::string payloadName, std::string mimetype, std::string data) {
    std::unique_ptr<ProcessingPayload> payload = std::make_unique<ProcessingPayload>();
    payload.get()->mimetype = mimetype;
    payload.get()->payloadData = std::make_unique<std::vector<uint8_t>>(std::vector<uint8_t>(data.begin(), data.end()));
    //namedPayloadData.insert(std::make_pair(payloadName, std::move(payload)));
}

std::optional<std::unique_ptr<ProcessingPayload>> PipelineProcessingData::getPayload(std::string payloadName) {
    auto search = namedPayloadData.find(payloadName);
    if(search != namedPayloadData.end()) {
        return std::optional<std::unique_ptr<ProcessingPayload>>(search->second.get());
    }
    return std::nullopt;
}