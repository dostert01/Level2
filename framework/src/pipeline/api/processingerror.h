#pragma once

#include <string>
#include "common.h"
#include "binaryprocessingdata.h"

namespace event_forge {

class ProcessingError : public BinaryProcessingData, public SerializableJson {
    public:
        ProcessingError(std::string errorCode, std::string errorMessage);
        ProcessingError(ProcessingError* other);
        std::string getErrorCode();
        std::string getErrorMessage();
        void toJson(void* jsonData) override;
        std::string errorCode;
        std::string errorMessage;
};

}