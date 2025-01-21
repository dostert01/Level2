#include <iostream>
#include <atomic>
#include "apistructs.h"
#include "payloadnames.h"
#include "common.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
#define JSON (*(json*)(jsonData))

using namespace std;

namespace event_forge {
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

string ProcessingPayload::getMimeType() {
    return mimetype;
}

void ProcessingPayload::setPayload(const string& payload) {
    this->stringPayloadData = payload;
}

void ProcessingPayload::setPayload(shared_ptr<BinaryProcessingData> payload) {
    this->binaryPayloadData = payload;
}

void to_json(nlohmann::json& j, const ProcessingPayload* p) {

    j = nlohmann::json{{"payloadName", p->payloadName}, {"mimetype", p->mimetype}, {"payload", p->stringPayloadData}};

    json parametersObject = json::object();
    for(const auto& [key, value] : ((PipelineProcessingData*)p)->getMatchingPatterns()) {
        parametersObject[key] = value;
    }
    j["parameters"] = parametersObject;
}

void from_json(const nlohmann::json& j, ProcessingPayload& p) {
    j.at("payloadName").get_to(p.payloadName);
    j.at("mimetype").get_to(p.mimetype);
    j.at("payload").get_to(p.stringPayloadData);
}

void ProcessingPayload::toJson(void* jsonData) {
    json j = this;
    JSON.push_back(j);
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
    payloadDataCollection.back()->setMatchingPatterns(*this);
}

void PipelineProcessingData::addPayloadData(string payloadName, string mimetype, shared_ptr<BinaryProcessingData> data) {
    payloadDataCollection.emplace_back(make_shared<ProcessingPayload>(payloadName, mimetype, data));
    payloadDataCollection.back()->setMatchingPatterns(*this);
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

vector<shared_ptr<ProcessingPayload>> PipelineProcessingData::getPayloads() {
    return payloadDataCollection;
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

void to_json(nlohmann::json& j, const PipelineProcessingData* p) {

    j = nlohmann::json{{"lastProcessedPipelineName", p->lastProcessedPipelineName}, {"processingCounter", p->processingCounter}};

    json processingPayloadArray = json::array();
    for(auto payload : ((PipelineProcessingData*)p)->getPayloads()) {
        payload->toJson(&processingPayloadArray);
    }
    j["processingPayloads"] = processingPayloadArray;

    json parametersObject = json::object();
    for(const auto& [key, value] : ((PipelineProcessingData*)p)->getMatchingPatterns()) {
        parametersObject[key] = value;
    }
    j["parameters"] = parametersObject;

    json processingErrorsArray = json::array();
    for(auto error : ((PipelineProcessingData*)p)->getAllErrors()) {
        error->toJson(&processingErrorsArray);
    }
    j["processingErrors"] = processingErrorsArray;

}

void from_json(const nlohmann::json& j, PipelineProcessingData& p) {
    j.at("lastProcessedPipelineName").get_to(p.lastProcessedPipelineName);
    j.at("processingCounter").get_to(p.processingCounter);
}

void PipelineProcessingData::toJson(void* jsonData) {
    json j = this;
    JSON.push_back(j);
}

/*
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
*/
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

void to_json(nlohmann::json& j, const ProcessingError* p) {
    j = nlohmann::json{{"errorMessage", p->errorMessage}, {"errorCode", p->errorCode}};
}

void from_json(const nlohmann::json& j, ProcessingError& p) {
    j.at("errorMessage").get_to(p.errorMessage);
    j.at("errorCode").get_to(p.errorCode);
}

void ProcessingError::toJson(void* jsonData) {
    json j = this;
    JSON.push_back(j);
}





} //namespace event_forge