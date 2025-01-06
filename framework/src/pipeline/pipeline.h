#ifndef SECOND_TAKE_PIPELINE
#define SECOND_TAKE_PIPELINE

#include <memory>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

#include "pipelineapi.h"
#include "matchable.h"

using json = nlohmann::json;

namespace event_forge {

#ifndef UNDEFINED_JSON_DATA
#define UNDEFINED_JSON_DATA "undefined"
#endif

class PipelineException : public std::exception {
    public:
        explicit PipelineException(const std::string& message);
        virtual const char* what() const noexcept override;
    private:
        std::string message;
}; 

using LibInitFunction = int (*)(PipelineStepInitData& initData);
using LibProcessFunction = int (*)(const PipelineProcessingData& processData);
using LibFinishFunction = int (*)();

class PipelineStep {
    public:
        PipelineStep() = default;
        ~PipelineStep();
        void setStepName(const std::string stepName);
        std::string getStepName();
        void setLibraryName(const std::string libraryName);
        bool isInitComplete();
        void runInitFunction();
        void runProcessingFunction(const PipelineProcessingData& processingData);
        void runFinishFunction();
        void loadLib();
        void loadNamedArguments(const json& jsonData);
    private:
        std::string stepName;
        std::string libraryName;
        void *hLib;
        LibInitFunction libInit;
        LibProcessFunction libProcess;
        LibFinishFunction libFinish;
        PipelineStepInitData initData;
};

class Pipeline : public Matchable {
    public:
        Pipeline() = default;
        static std::shared_ptr<Pipeline> getInstance();
        static std::optional<std::shared_ptr<Pipeline>> getInstance(const std::string configFilePath);
        uint getCountOfPipelineSteps();
        std::optional<std::shared_ptr<PipelineStep>> getStepByName(const std::string& stepName);
        void setPipelineName(const std::string& pipelineName);
        std::string getPipelineName();
        void execute();
        void execute(PipelineProcessingData& processData);
    private:
        std::string pipelineName;
        std::vector<std::shared_ptr<PipelineStep>> pipelineSteps;
        void loadPipelineConfig(const std::string& configFilePath);
        void loadPipelineMetaData(const json& jsonData);
        void loadPipelineSteps(const json& jsonData);
        void tagProcessingData(PipelineProcessingData& processData);
};

}

#endif // SECOND_TAKE_PIPELINE