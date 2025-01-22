#pragma once

#include <string>
#include <memory>

#include "common.h"
#include "binaryprocessingdata.h"
#include "matchable.h"

namespace event_forge {

class ProcessingPayload : public Matchable, public SerializableJson {
    private:
        std::shared_ptr<BinaryProcessingData> binaryPayloadData;
    public:
        std::string payloadName;
        std::string mimetype;
        std::string stringPayloadData;
        ProcessingPayload() = default;
        ProcessingPayload(std::string payloadName, std::string mimetype, const std::string& payload);
        ProcessingPayload(std::string payloadName, std::string mimetype, std::shared_ptr<BinaryProcessingData> payload);
        ~ProcessingPayload();
        void setPayloadName(std::string payloadName);
        std::string getPayloadName();
        void setMimeType(std::string mimetype);
        std::string getMimeType();
        void setPayload(const std::string& payload);
        void setPayload(std::shared_ptr<BinaryProcessingData> payload);
        std::string payloadAsString();
        std::string payloadAsBase64String();
        std::shared_ptr<BinaryProcessingData> payloadAsBinaryData();
        void toJson(void* jsonData) override;
};

}
