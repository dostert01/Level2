#pragma once

#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include <vector>
#include <thread>
#include <atomic>
#include <shared_mutex>

#include "pipeline.h"
#include "pipelinefifo.h"

namespace level2 {

    class PipeLineProcessor {
        public:
            PipeLineProcessor() = default;
            ~PipeLineProcessor();
            static std::shared_ptr<PipeLineProcessor> getInstance();
            static std::optional<std::shared_ptr<PipeLineProcessor>> getInstance(const std::string configFilePath);
            uint getCountOfPipelines();
            std::string getProcessName();
            std::optional<std::shared_ptr<Pipeline>> getPipelineByName(std::string pipelineName);
            void execute(std::shared_ptr<PipelineProcessingData> payload);
            void startProcessingLoop(std::shared_ptr<PipelineFiFo> fifo);
            bool isProcessingLoopRunning();
            void stopProcessingLoop();
        private:
            std::string configFileDir;
            std::string processName;
            std::thread processingLoopThread;
            std::atomic_bool keepThreadRunning;
            std::shared_mutex pipelineExecutionMutex;
            std::vector<std::shared_ptr<Pipeline>> pipelines;
            void loadProcessorConfig(const std::string& configFilePath);
            void setConfigFileDirFromConfigFileName(
                const std::string& configFilePath);
            void loadHeaderData(const json& jsonData);
            void loadPipelines(const json& jsonData);
            std::string getDirNameFromPath(const std::string path);
            std::string getPipelineConfigFileNameFromJsonData(const nlohmann::json_abi_v3_11_3::json &currentPipelineDefinition);
            void readMatchingPatternsFromJson(std::shared_ptr<Pipeline> pipeline, const json& pipelineDefinition);
            void processingLoop(std::shared_ptr<PipelineFiFo> fifo);
    };

}  // namespace level2

