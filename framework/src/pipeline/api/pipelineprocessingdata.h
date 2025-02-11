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

/**
 * @class PipelineProcessingData
 * @brief This class is used to pass instances of `ProcessingPayload` through pipelines
 *
 * `PipelineProcessingData` mainly acts as a container for instances of `ProcessingPayload`
 * as they are passed though the pipeline respectively through the exported
 * function `pipeline_step_module_process(level2::PipelineProcessingData& processData)`
 * of any working module involved in the execution of a pipeline.
 * The function is declared in file `pipelineapi.h` and must be implemented by any
 * worker module.
 * 
 * See tests in `test_pipelineprocessor.cpp` and any (example) worker
 * module like `helloWorld.cpp` for usage examples.
 * 
 */
class PipelineProcessingData : public Matchable, public SerializableJson {
    public:
        PipelineProcessingData();
        ~PipelineProcessingData();
        /**
         * @brief Counts involvements in pipelines
         * 
         * Used by class `Pipeline` through `increaseProcessingCounter()`
         * to count the amount of pipelines in which one instance of this
         * class has been processed.
         * 
         * Kept public just because its easier for use of this variable in
         * json serialization. 
         */
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
        /**
         * @brief Instance counter
         * 
         * This variable is used to count the instances of PipelineProcessingData,
         * that have been created during runtime of the program.
         * 
         * Currently its only use case it during creation of a unique transaction id
         * in `PipelineProcessingData::setDefaultProperties()`.
         */
        static std::atomic_int instanceCounter;
        std::vector<std::shared_ptr<ProcessingPayload>> payloadDataCollection;
        std::vector<std::shared_ptr<ProcessingPayload>> errors;
        void setDefaultProperties();
        std::string getTimeStampOfNow(const std::string& pattern);
        std::string getFormattedCounter();
};

}
