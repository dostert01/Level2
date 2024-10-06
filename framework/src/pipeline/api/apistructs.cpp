#include "apistructs.h"
#include <iostream>

std::optional<std::string> PipelineStepInitData::getNamedArgument(const std::string& argumentName) {
    auto search = namedArguments.find(argumentName);
    if(search != namedArguments.end()) {
        return search->second;
    }
    return std::nullopt;
}

std::string ProcessingPayload::payloadAsString() {
    std::string s(payloadData.get()->begin(), payloadData.get()->end());
    return s;
}

PipelineProcessingData::~PipelineProcessingData() {
    std::multimap<std::string, ProcessingPayload*>::iterator it;
    for(it = namedPayloadData.begin(); it != namedPayloadData.end(); ++it) {
        if(it->second != NULL) {
            std::cout << "deleting payload of name: " << it->first << std::endl;
            delete it->second;
        }
    }
}

void PipelineProcessingData::addPayloadData(std::string payloadName, std::string mimetype, std::string data) {
    ProcessingPayload* payload = new ProcessingPayload();
    payload->mimetype = mimetype;
    payload->payloadData = std::make_unique<std::vector<uint8_t>>(std::vector<uint8_t>(data.begin(), data.end()));
    namedPayloadData.insert(std::make_pair(payloadName, std::move(payload)));
}

std::optional<ProcessingPayload*> PipelineProcessingData::getPayload(std::string payloadName) {
    auto search = namedPayloadData.find(payloadName);
    if(search != namedPayloadData.end()) {
        return search->second;
    }
    return std::nullopt;
}