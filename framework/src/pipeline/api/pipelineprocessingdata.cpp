#include "pipelineprocessingdata.h"
#include "payloadnames.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#define JSON (*(json*)(jsonData))

namespace level2 {

std::atomic_int PipelineProcessingData::instanceCounter{0};

PipelineProcessingData::PipelineProcessingData() {
    setDefaultProperties();
}

std::shared_ptr<PipelineProcessingData> PipelineProcessingData::getInstance() {
    return std::make_shared<PipelineProcessingData>();
}

PipelineProcessingData::~PipelineProcessingData() {}

void PipelineProcessingData::setDefaultProperties() {
    
    addMatchingPattern(PAYLOAD_MATCHING_PATTERN_TRANSACTION_ID, getTimeStampOfNow("%Y%m%d%H%M%S") +
        "_" + getFormattedCounter());
    addMatchingPattern(PAYLOAD_MATCHING_PATTERN_DATE_CREATED, getTimeStampOfNow("%Y%m%d"));
    addMatchingPattern(PAYLOAD_MATCHING_PATTERN_TIME_CREATED, getTimeStampOfNow("%H%M%S"));
    setLastProcessedPipelineName("");
}

std::string PipelineProcessingData::getFormattedCounter() {
    char buffer[1024];
    int localCounter = instanceCounter.fetch_add(1);
    if(localCounter > PIPELINE_PROCESSING_DATA_COUNTER_MAX) {
        instanceCounter.store(0);
    }
    sprintf(buffer, "%09d", localCounter);
    return std::string(buffer);
}

std::string PipelineProcessingData::getTimeStampOfNow(const std::string& pattern) {
  time_t timeBuffer;
  struct tm *localTime;
  char buffer[64];

  time(&timeBuffer);
  localTime = localtime(&timeBuffer);
  strftime(buffer, sizeof(buffer), pattern.c_str(), localTime);
  std::string str(buffer);
  return str;
}

void PipelineProcessingData::addPayloadData(std::string payloadName, std::string mimetype, const std::string& data) {
    payloadDataCollection.emplace_back(std::make_shared<ProcessingPayload>(payloadName, mimetype, data));
    payloadDataCollection.back()->setMatchingPatterns(*this);
}

void PipelineProcessingData::addPayloadData(std::string payloadName, std::string mimetype, std::shared_ptr<BinaryProcessingData> data) {
    payloadDataCollection.emplace_back(std::make_shared<ProcessingPayload>(payloadName, mimetype, data));
    payloadDataCollection.back()->setMatchingPatterns(*this);
}

void PipelineProcessingData::addError(std::string errorCode, std::string errorMessage) {
    auto error = std::make_shared<ProcessingError>(errorCode, errorMessage);
    errors.emplace_back(std::make_shared<ProcessingPayload>(PAYLOAD_NAME_PROCESSING_ERROR, PAYLOAD_MIMETYPE_APPLICATION_OCTET_STREAM, error));
}

bool PipelineProcessingData::hasError() {
    return (!errors.empty());
}

std::vector<std::shared_ptr<ProcessingError>> PipelineProcessingData::getAllErrors() {
    std::vector<std::shared_ptr<ProcessingError>> returnVector;
    for(auto payload : errors) {
        if(payload->getPayloadName().compare(PAYLOAD_NAME_PROCESSING_ERROR) == 0) {
            std::shared_ptr<ProcessingError> error = dynamic_pointer_cast<ProcessingError>(payload->payloadAsBinaryData());
            if(error)
                returnVector.push_back(error);
        }
    }
    return returnVector;
}

std::vector<std::shared_ptr<ProcessingPayload>> PipelineProcessingData::getPayloads() {
    return payloadDataCollection;
}

std::optional<std::shared_ptr<ProcessingPayload>> PipelineProcessingData::getPayload(std::string payloadName) {
    std::optional<std::shared_ptr<ProcessingPayload>> returnValue = std::nullopt;
    for(auto payload : payloadDataCollection) {
        if(payload->getPayloadName().compare(payloadName) == 0) {
            returnValue = payload;
        }
    }
    return returnValue;
}

std::optional<std::shared_ptr<ProcessingPayload>> PipelineProcessingData::getLastPayload() {
    std::optional<std::shared_ptr<ProcessingPayload>> returnValue = std::nullopt;
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

std::string PipelineProcessingData::getFormattedProcessingCounter() {
    char buffer[1024];
    sprintf(buffer, "%05d", getProcessingCounter());
    return std::string(buffer);
}

void PipelineProcessingData::increaseProcessingCounter() {
    processingCounter++;
}

void PipelineProcessingData::setLastProcessedPipelineName(std::string pipelineName) {
    addMatchingPattern(PAYLOAD_MATCHING_PATTERN_LAST_PIPELINE, pipelineName);
}

std::string PipelineProcessingData::getLastProcessedPipelineName() {
    return getMatchingPattern(PAYLOAD_MATCHING_PATTERN_LAST_PIPELINE).value_or("");
}

void to_json(nlohmann::json& j, const PipelineProcessingData* p) {

    j = nlohmann::json{{"processingCounter", p->processingCounter}};

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
    j.at("processingCounter").get_to(p.processingCounter);
}

void PipelineProcessingData::toJson(void* jsonData) {
    json j = this;
    JSON.push_back(j);
}

}
