#include <iostream>
#include <atomic>
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
ProcessingPayload::ProcessingPayload(string payloadName, string mimetype, const string& payload) {
    setPayloadName(payloadName);
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::ProcessingPayload(string payloadName, string mimetype, shared_ptr<BinaryProcessingData> payload){
    setPayloadName(payloadName);
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

string ProcessingPayload::getPayloadName() {
    return payloadName;
}

void ProcessingPayload::setPayloadName(string payloadName) {
    this->payloadName = payloadName;
}

void ProcessingPayload::setMimeType(string mimetype) {
    this->mimetype = mimetype;
}

void ProcessingPayload::setPayload(const string& payload) {
    this->stringPayloadData = payload;
}

void ProcessingPayload::setPayload(shared_ptr<BinaryProcessingData> payload) {
    this->binaryPayloadData = payload;
}

/*
    PipelineProcessingData
*/

atomic_int PipelineProcessingData::counter{0};

PipelineProcessingData::PipelineProcessingData() {
    setDefaultProperties();
}

shared_ptr<PipelineProcessingData> PipelineProcessingData::getInstance() {
    return make_shared<PipelineProcessingData>();
}

PipelineProcessingData::~PipelineProcessingData() {}

void PipelineProcessingData::setDefaultProperties() {
    
    addMatchingPattern(PAYLOAD_MATCHING_PATTERN_TRANSACTION_ID, getTimeStampOfNow("%Y%m%d%H%M%S") +
        "_" + getFormattedCounter());
    addMatchingPattern(PAYLOAD_MATCHING_PATTERN_DATE_CREATED, getTimeStampOfNow("%Y%m%d"));
    addMatchingPattern(PAYLOAD_MATCHING_PATTERN_TIME_CREATED, getTimeStampOfNow("%H%M%S"));
}

string PipelineProcessingData::getFormattedCounter() {
    char buffer[1024];
    int localCounter = counter.fetch_add(1);
    if(localCounter > PIPELINE_PROCESSING_DATA_COUNTER_MAX) {
        counter.store(0);
    }
    sprintf(buffer, "%09d", localCounter);
    return string(buffer);
}

string PipelineProcessingData::getTimeStampOfNow(const string& pattern) {
  time_t timeBuffer;
  struct tm *localTime;
  char buffer[64];

  time(&timeBuffer);
  localTime = localtime(&timeBuffer);
  strftime(buffer, sizeof(buffer), pattern.c_str(), localTime);
  string str(buffer);
  return str;
}

void PipelineProcessingData::addPayloadData(string payloadName, string mimetype, const string& data) {
    payloadDataCollection.emplace_back(make_shared<ProcessingPayload>(payloadName, mimetype, data));
}

void PipelineProcessingData::addPayloadData(string payloadName, string mimetype, shared_ptr<BinaryProcessingData> data) {
    payloadDataCollection.emplace_back(make_shared<ProcessingPayload>(payloadName, mimetype, data));
}

void PipelineProcessingData::addError(string errorCode, string errorMessage) {
    auto error = make_shared<ProcessingError>(errorCode, errorMessage);
    errors.emplace_back(make_shared<ProcessingPayload>(PAYLOAD_NAME_PROCESSING_ERROR, PAYLOAD_MIMETYPE_APPLICATION_OCTET_STREAM, error));
}

bool PipelineProcessingData::hasError() {
    return (!errors.empty());
}

vector<shared_ptr<ProcessingError>> PipelineProcessingData::getAllErrors() {
    vector<shared_ptr<ProcessingError>> returnVector;
    for(auto payload : errors) {
        if(payload->getPayloadName().compare(PAYLOAD_NAME_PROCESSING_ERROR) == 0) {
            shared_ptr<ProcessingError> error = dynamic_pointer_cast<ProcessingError>(payload->payloadAsBinaryData());
            if(error)
                returnVector.push_back(error);
        }
    }
    return returnVector;
}

optional<shared_ptr<ProcessingPayload>> PipelineProcessingData::getPayload(string payloadName) {
    optional<shared_ptr<ProcessingPayload>> returnValue = nullopt;
    for(auto payload : payloadDataCollection) {
        if(payload->getPayloadName().compare(payloadName) == 0) {
            returnValue = payload;
        }
    }
    return returnValue;
}

optional<shared_ptr<ProcessingPayload>> PipelineProcessingData::getLastPayload() {
    optional<shared_ptr<ProcessingPayload>> returnValue = nullopt;
    if(!payloadDataCollection.empty()) {
        returnValue = payloadDataCollection.back();
    }
    return returnValue;
}

int PipelineProcessingData::getCountOfPayloads() {
    return payloadDataCollection.size();
}

int PipelineProcessingData::getProcessingCounter() {
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
