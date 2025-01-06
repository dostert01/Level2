#ifndef SECOND_TAKE_PIPELINE_PROCESSOR
#define SECOND_TAKE_PIPELINE_PROCESSOR

#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include <vector>

#include "pipeline.h"

namespace event_forge {

    class PipeLineProcessor {
        public:
            PipeLineProcessor() = default;
            ~PipeLineProcessor();
            static std::unique_ptr<PipeLineProcessor> getInstance();
            static std::optional<std::unique_ptr<PipeLineProcessor>> getInstance(const std::string configFilePath);
            uint getCountOfPipelines();
            std::string getProcessName();
            std::optional<std::shared_ptr<Pipeline>> getPipelineByName(std::string pipelineName);
            void execute(PipelineProcessingData& payload);
        private:
            std::string configFileDir;
            std::string processName;
            std::vector<std::shared_ptr<Pipeline>> pipelines;
            void loadProcessorConfig(const std::string& configFilePath);
            void setConfigFileDirFromConfigFileName(
                const std::string& configFilePath);
            void loadHeaderData(const json& jsonData);
            void loadPipelines(const json& jsonData);
            std::string getDirNameFromPath(const std::string path);
            std::string getPipelineConfigFileNameFromJsonData(const nlohmann::json_abi_v3_11_3::json &currentPipelineDefinition);
            void readMatchingPatternsFromJson(std::shared_ptr<Pipeline> pipeline, const json& pipelineDefinition);
    };

}  // namespace event_forge

#endif  // SECOND_TAKE_PIPELINE_PROCESSOR
