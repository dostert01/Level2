#include <iostream>
#include <atomic>
#include "apistructs.h"
#include "payloadnames.h"
#include "common.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
#define JSON (*(json*)(jsonData))

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

string ProcessingPayload::payloadAsBase64String() {
    return event_forge::Base64Encoder::encodeNoNewLines(payloadAsString());
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

void ProcessingPayload::toJson(void* jsonData) {
    std::cout << "ProcessingPayload::toJson" << std::endl;
    auto j = JSON.find("processingPayloads");
    std::cout << "ProcessingPayload::toJson 01" << std::endl;
    if(j == JSON.end()) {
        std::cout << "ProcessingPayload::toJson 02" << std::endl;
        JSON.emplace_back(json::object_t::value_type("processingPayloads", {{"hallo", 1}}));
        std::cout << "ProcessingPayload::toJson 02a" << std::endl;
        j = JSON.find("processingPayloads");
    }
    std::cout << "ProcessingPayload::toJson 03" << std::endl;
    j.value().push_back(
        {{"payloadName", payloadName}, {"mimetype", mimetype}, {"payload", payloadAsBase64String()}});
    std::cout << "ProcessingPayload::toJson 04" << std::endl;
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

void PipelineProcessingData::toJson(void* jsonData) {
    std::cout << "PipelineProcessingData::toJson" << std::endl;
    auto j = JSON.find("PipelineProcessingDatas");
    if(j == JSON.end()) {
        JSON.push_back(json::object_t::value_type("PipelineProcessingDatas", {}));
        j = JSON.find("PipelineProcessingDatas");
    }
    j.value().push_back(
        {
            {"lastProcessedPipelineName", lastProcessedPipelineName},
            {"processingCounter", processingCounter},
            {"bla", "fasel"}
        });
    //JSON.push_back(json::object_t::value_type("ProcessingPayloads", {}));
    //    j = JSON.find("ProcessingPayloads");
    for(auto currentProcessingPayload : payloadDataCollection) {
        currentProcessingPayload->toJson(&j);
    }

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

void ProcessingError::toJson(void* jsonData) {
    std::cout << "ProcessingError::toJson" << std::endl;
    auto j = JSON.find("processingErrors");
    if(j == JSON.end()) {
        JSON.push_back(json::object_t::value_type("processingErrors", {}));
        j = JSON.find("processingErrors");
    }
    j.value().push_back(
        {{"errorMessage", errorMessage}, {"errorCode", errorCode}});
}