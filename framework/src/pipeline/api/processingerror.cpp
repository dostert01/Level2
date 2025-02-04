#include "processingerror.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
#define JSON (*(json*)(jsonData))

namespace level2 {

ProcessingError::ProcessingError(std::string errorCode, std::string errorMessage) : BinaryProcessingData() {
    this->errorCode = errorCode;
    this->errorMessage = errorMessage;
}

ProcessingError::ProcessingError(ProcessingError* other) : BinaryProcessingData() {
    errorCode = other->getErrorCode();
    errorMessage = other->getErrorMessage();
}

std::string ProcessingError::getErrorCode() {
    return errorCode;
}

std::string ProcessingError::getErrorMessage() {
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

}