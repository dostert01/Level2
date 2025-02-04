#include "processingpayload.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#define JSON (*(json*)(jsonData))

namespace level2 {

ProcessingPayload::ProcessingPayload(std::string payloadName, std::string mimetype, const std::string& payload) {
    setPayloadName(payloadName);
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::ProcessingPayload(std::string payloadName, std::string mimetype, std::shared_ptr<BinaryProcessingData> payload){
    setPayloadName(payloadName);
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::~ProcessingPayload() {}

std::string ProcessingPayload::payloadAsString() {
    return stringPayloadData;
}

std::string ProcessingPayload::payloadAsBase64String() {
    return level2::Base64Encoder::encodeNoNewLines(payloadAsString());
}

std::shared_ptr<BinaryProcessingData> ProcessingPayload::payloadAsBinaryData() {
    return binaryPayloadData;
}

std::string ProcessingPayload::getPayloadName() {
    return payloadName;
}

void ProcessingPayload::setPayloadName(std::string payloadName) {
    this->payloadName = payloadName;
}

void ProcessingPayload::setMimeType(std::string mimetype) {
    this->mimetype = mimetype;
}

std::string ProcessingPayload::getMimeType() {
    return mimetype;
}

void ProcessingPayload::setPayload(const std::string& payload) {
    this->stringPayloadData = payload;
}

void ProcessingPayload::setPayload(std::shared_ptr<BinaryProcessingData> payload) {
    this->binaryPayloadData = payload;
}

void to_json(nlohmann::json& j, const ProcessingPayload* p) {

    std::string base64Payload = level2::Base64Encoder::encodeNoNewLines(p->stringPayloadData);
    j = nlohmann::json{{"payloadName", p->payloadName}, {"mimetype", p->mimetype}, {"payload", base64Payload}};

    json parametersObject = json::object();
    for(const auto& [key, value] : ((ProcessingPayload*)p)->getMatchingPatterns()) {
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

}