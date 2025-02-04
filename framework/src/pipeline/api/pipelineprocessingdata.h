#pragma once

#include <string>
#include <atomic>
#include <vector>
#include <limits.h>
#include "matchable.h"
#include "common.h"
#include "processingerror.h"
#include "processingpayload.h"

#define PIPELINE_PROCESSING_DATA_COUNTER_MAX (INT_MAX / 2)

namespace level2 {

class PipelineProcessingData : public Matchable, public SerializableJson {
    public:
        PipelineProcessingData();
        ~PipelineProcessingData();
        int processingCounter = 0;
        static std::shared_ptr<PipelineProcessingData> getInstance();
        void addPayloadData(std::string payloadName, std::string mimetype, const std::string& data);
        void addPayloadData(std::string payloadName, std::string mimetype, std::shared_ptr<BinaryProcessingData> data);
        void addError(std::string errorCode, std::string errorMessage);
        bool hasError();
        std::vector<std::shared_ptr<ProcessingError>> getAllErrors();
        std::vector<std::shared_ptr<ProcessingPayload>> getPayloads();
        std::optional<std::shared_ptr<ProcessingPayload>> getPayload(std::string payloadName);
        std::optional<std::shared_ptr<ProcessingPayload>> getLastPayload();
        int getCountOfPayloads();
        int getProcessingCounter();
        std::string getFormattedProcessingCounter();
        void increaseProcessingCounter();
        void setLastProcessedPipelineName(std::string pipelineName);
        std::string getLastProcessedPipelineName();
        void toJson(void* jsonData) override;
    private:
        static std::atomic_int counter;
        std::vector<std::shared_ptr<ProcessingPayload>> payloadDataCollection;
        std::vector<std::shared_ptr<ProcessingPayload>> errors;
        void setDefaultProperties();
        std::string getTimeStampOfNow(const std::string& pattern);
        std::string getFormattedCounter();
};

}
