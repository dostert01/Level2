#include "apistructs.h"
#include <iostream>

/*
    BinaryProcessingData
*/
BinaryProcessingData::~BinaryProcessingData() {}

/*
    PipelineStepInitData
*/
std::optional<std::string> PipelineStepInitData::getNamedArgument(const std::string& argumentName) {
    auto search = namedArguments.find(argumentName);
    if(search != namedArguments.end()) {
        return search->second;
    }
    return std::nullopt;
}

/*
    ProcessingPayload
*/
ProcessingPayload::ProcessingPayload(std::string mimetype, std::string payload) {
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::ProcessingPayload(std::string mimetype, std::shared_ptr<BinaryProcessingData> payload){
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::~ProcessingPayload() {}

std::string ProcessingPayload::payloadAsString() {
    return stringPayloadData;
}

std::shared_ptr<BinaryProcessingData> ProcessingPayload::payloadAsBinaryData() {
    return binaryPayloadData;
}

void ProcessingPayload::setMimeType(std::string mimetype) {
    this->mimetype = mimetype;
}

void ProcessingPayload::setPayload(std::string payload) {
    this->stringPayloadData = payload;
}

void ProcessingPayload::setPayload(std::shared_ptr<BinaryProcessingData> payload) {
    this->binaryPayloadData = payload;
}

/*
    PipelineProcessingData
*/
PipelineProcessingData::~PipelineProcessingData() {
    /*
    std::multimap<std::string, std::shared_ptr<ProcessingPayload>>::iterator it;
    for(it = namedPayloadData.begin(); it != namedPayloadData.end(); ++it) {
        if(it->second != NULL) {
            std::cout << "deleting payload of name: " << it->first << std::endl;
            delete it->second;
        }
    }
    */
}

void PipelineProcessingData::addPayloadData(std::string payloadName, std::string mimetype, std::string data) {
    std::shared_ptr<ProcessingPayload> payload = std::make_shared<ProcessingPayload>(mimetype, data);
    namedPayloadData.insert(std::make_pair(payloadName, std::move(payload)));
}

void PipelineProcessingData::addPayloadData(std::string payloadName, std::string mimetype, std::shared_ptr<BinaryProcessingData> data) {
    std::shared_ptr<ProcessingPayload> payload = std::make_shared<ProcessingPayload>(mimetype, data);
    namedPayloadData.insert(std::make_pair(payloadName, std::move(payload)));
}

std::optional<std::shared_ptr<ProcessingPayload>> PipelineProcessingData::getPayload(std::string payloadName) {
    auto search = namedPayloadData.find(payloadName);
    if(search != namedPayloadData.end()) {
        return search->second;
    }
    return std::nullopt;
}

uint PipelineProcessingData::getCountOfPayloads() {
    return namedPayloadData.size();
}

uint PipelineProcessingData::getProcessingCounter() {
    return processingCounter;
}

void PipelineProcessingData::increaseProcessingCounter() {
    processingCounter++;
}

void PipelineProcessingData::setLastProcessedPipelineName(std::string pipelineName) {
    lastProcessedPipelineName = pipelineName;
}

std::string PipelineProcessingData::getLastProcessedPipelineName() {
    return lastProcessedPipelineName;
}
