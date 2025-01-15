#ifndef SECOND_TAKE_PIPELINE
#define SECOND_TAKE_PIPELINE

#include <memory>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

#include "pipelineapi.h"
#include "matchable.h"

using json = nlohmann::json;
using namespace std;
namespace event_forge {

#ifndef UNDEFINED_JSON_DATA
#define UNDEFINED_JSON_DATA "undefined"
#endif

class PipelineException : public exception {
    public:
        explicit PipelineException(const string& message);
        virtual const char* what() const noexcept override;
    private:
        string message;
}; 

using LibInitFunction = int (*)(PipelineStepInitData& initData);
using LibProcessFunction = int (*)(const PipelineProcessingData& processData);
using LibFinishFunction = int (*)();

class PipelineStep {
    public:
        PipelineStep() = default;
        ~PipelineStep();
        void setStepName(const string stepName);
        string getStepName();
        void setLibraryName(const string libraryName);
        bool isInitComplete();
        void runInitFunction();
        void runProcessingFunction(shared_ptr<PipelineProcessingData> processingData);
        void runFinishFunction();
        void loadLib();
        void loadNamedArguments(const json& jsonData);
    private:
        string stepName;
        string libraryName;
        void *hLib;
        LibInitFunction libInit;
        LibProcessFunction libProcess;
        LibFinishFunction libFinish;
        PipelineStepInitData initData;
};

class Pipeline : public Matchable {
    public:
        Pipeline() = default;
        static shared_ptr<Pipeline> getInstance();
        static optional<shared_ptr<Pipeline>> getInstance(const string configFilePath);
        uint getCountOfPipelineSteps();
        optional<shared_ptr<PipelineStep>> getStepByName(const string& stepName);
        void setPipelineName(const string& pipelineName);
        string getPipelineName();
        void execute();
        void execute(shared_ptr<PipelineProcessingData> processData);
    private:
        string pipelineName;
        vector<shared_ptr<PipelineStep>> pipelineSteps;
        void loadPipelineConfig(const string& configFilePath);
        void loadPipelineMetaData(const json& jsonData);
        void loadPipelineSteps(const json& jsonData);
        void tagProcessingData(shared_ptr<PipelineProcessingData> processData);
};

}

#endif // SECOND_TAKE_PIPELINE