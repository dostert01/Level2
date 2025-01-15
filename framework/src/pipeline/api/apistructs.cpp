#include <iostream>
#include "apistructs.h"
#include "payloadnames.h"

using namespace std;

/*
    BinaryProcessingData
*/
BinaryProcessingData::~BinaryProcessingData() {}

/*
    PipelineStepInitData
*/
optional<string> PipelineStepInitData::getNamedArgument(const string& argumentName) {
    auto search = namedArguments.find(argumentName);
    if(search != namedArguments.end()) {
        return search->second;
    }
    return nullopt;
}

/*
    ProcessingPayload
*/
ProcessingPayload::ProcessingPayload(string mimetype, string payload) {
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::ProcessingPayload(string mimetype, shared_ptr<BinaryProcessingData> payload){
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::~ProcessingPayload() {}

string ProcessingPayload::payloadAsString() {
    return stringPayloadData;
}

shared_ptr<BinaryProcessingData> ProcessingPayload::payloadAsBinaryData() {
    return binaryPayloadData;
}

void ProcessingPayload::setMimeType(string mimetype) {
    this->mimetype = mimetype;
}

void ProcessingPayload::setPayload(string payload) {
    this->stringPayloadData = payload;
}

void ProcessingPayload::setPayload(shared_ptr<BinaryProcessingData> payload) {
    this->binaryPayloadData = payload;
}

/*
    PipelineProcessingData
*/
shared_ptr<PipelineProcessingData> PipelineProcessingData::getInstance() {
    return make_shared<PipelineProcessingData>();
}

PipelineProcessingData::~PipelineProcessingData() {}

void PipelineProcessingData::addPayloadData(string payloadName, string mimetype, string data) {
    auto payload = make_shared<ProcessingPayload>(mimetype, data);
    namedPayloadData.insert(make_pair(payloadName, payload));
}

void PipelineProcessingData::addPayloadData(string payloadName, string mimetype, shared_ptr<BinaryProcessingData> data) {
    auto payload = make_shared<ProcessingPayload>(mimetype, data);
    namedPayloadData.insert(make_pair(payloadName, payload));
}

void PipelineProcessingData::addError(string errorCode, string errorMessage) {
    auto error = make_shared<ProcessingError>(errorCode, errorMessage);
    addPayloadData(PAYLOAD_NAME_PROCESSING_ERROR, PAYLOAD_MIMETYPE_APPLICATION_OCTET_STREAM, error);
}

bool PipelineProcessingData::hasError() {
    return namedPayloadData.contains(PAYLOAD_NAME_PROCESSING_ERROR);
}

vector<ProcessingError> PipelineProcessingData::getAllErrors() {
    vector<ProcessingError> returnVector;
    if(hasError()) {
        auto allErrors = namedPayloadData.equal_range(PAYLOAD_NAME_PROCESSING_ERROR);
        for(auto iterator = allErrors.first; iterator != allErrors.second; ++iterator) {
            ProcessingError* error = (ProcessingError*)(iterator->second.get()->payloadAsBinaryData().get());
            returnVector.emplace_back(error);
        }
    }
    return returnVector;
}

optional<shared_ptr<ProcessingPayload>> PipelineProcessingData::getPayload(string payloadName) {
    auto search = namedPayloadData.find(payloadName);
    if(search != namedPayloadData.end()) {
        return search->second;
    }
    return nullopt;
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

void PipelineProcessingData::setLastProcessedPipelineName(string pipelineName) {
    lastProcessedPipelineName = pipelineName;
}

string PipelineProcessingData::getLastProcessedPipelineName() {
    return lastProcessedPipelineName;
}

/*
    ProcessingError
*/
ProcessingError::ProcessingError(string errorCode, string errorMessage) : BinaryProcessingData() {
    this->errorCode = errorCode;
    this->errorMessage = errorMessage;
}

ProcessingError::ProcessingError(ProcessingError* other) : BinaryProcessingData() {
    errorCode = other->getErrorCode();
    errorMessage = other->getErrorMessage();
}

string ProcessingError::getErrorCode() {
    return errorCode;
}

string ProcessingError::getErrorMessage() {
    return errorMessage;
}
