#ifndef SECOND_TAKE_PIPELINE_PROCESSOR
#define SECOND_TAKE_PIPELINE_PROCESSOR

#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include <vector>

#include "pipeline.h"

namespace second_take {

    class PipeLineProcessor {
        public:
            PipeLineProcessor() = default;
            ~PipeLineProcessor();
            static std::unique_ptr<PipeLineProcessor> getInstance();
            static std::optional<std::unique_ptr<PipeLineProcessor>> getInstance(const std::string configFilePath);
            uint getCountOfPipelines();
            std::string getProcessName();
        private:
            std::string configFileDir;
            std::string processName;
            std::vector<std::unique_ptr<second_take::Pipeline>> pipelines;
            void loadProcessorConfig(const std::string& configFilePath);
            void setConfigFileDirFromConfigFileName(
                const std::string& configFilePath);
            void loadHeaderData(const json& jsonData);
            void loadPipelines(const json& jsonData);
            std::string getDirNameFromPath(const std::string path);
            std::string getPipelineConfigFileNameFromJsonData(const nlohmann::json_abi_v3_11_3::json &currentPipelineDefinition);
    };

}  // namespace second_take

#endif  // SECOND_TAKE_PIPELINE_PROCESSOR
