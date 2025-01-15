#ifndef SECOND_TAKE_PIPELINE_PROCESSOR
#define SECOND_TAKE_PIPELINE_PROCESSOR

#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include <vector>
#include <thread>
#include <atomic>
#include <shared_mutex>

#include "pipeline.h"
#include "pipelinefifo.h"

using namespace std;
namespace event_forge {

    class PipeLineProcessor {
        public:
            PipeLineProcessor() = default;
            ~PipeLineProcessor();
            static unique_ptr<PipeLineProcessor> getInstance();
            static optional<unique_ptr<PipeLineProcessor>> getInstance(const string configFilePath);
            uint getCountOfPipelines();
            string getProcessName();
            optional<shared_ptr<Pipeline>> getPipelineByName(string pipelineName);
            void execute(shared_ptr<PipelineProcessingData> payload);
            void startProcessingLoop(shared_ptr<PipelineFiFo> fifo);
            bool isProcessingLoopRunning();
            void stopProcessingLoop();
        private:
            string configFileDir;
            string processName;
            thread processingLoopThread;
            atomic_bool keepThreadRunning;
            shared_mutex pipelineExecutionMutex;
            vector<shared_ptr<Pipeline>> pipelines;
            void loadProcessorConfig(const string& configFilePath);
            void setConfigFileDirFromConfigFileName(
                const string& configFilePath);
            void loadHeaderData(const json& jsonData);
            void loadPipelines(const json& jsonData);
            string getDirNameFromPath(const string path);
            string getPipelineConfigFileNameFromJsonData(const nlohmann::json_abi_v3_11_3::json &currentPipelineDefinition);
            void readMatchingPatternsFromJson(shared_ptr<Pipeline> pipeline, const json& pipelineDefinition);
            void processingLoop(shared_ptr<PipelineFiFo> fifo);
    };

}  // namespace event_forge

#endif  // SECOND_TAKE_PIPELINE_PROCESSOR
