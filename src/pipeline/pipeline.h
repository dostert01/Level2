#ifndef SECOND_TAKE_PIPELINE
#define SECOND_TAKE_PIPELINE

#include <memory>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace second_take {

class PipelineException : public std::exception {
    public:
        explicit PipelineException(const std::string& message);
        //virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW  noexcept override;
        virtual const char* what() const noexcept override;
    private:
        std::string message;
}; 

class PipelineStep {
    public:
        PipelineStep() = default;
        void setStepName(const std::string stepName);
        void setLibraryName(const std::string libraryName);
        bool isInitComplete();
    private:
        std::string stepName;
        std::string libraryName;
};

class Pipeline {
    public:
        Pipeline() = default;
        static std::unique_ptr<Pipeline> getInstance();
        static std::optional<std::unique_ptr<Pipeline>> getInstance(const std::string configFilePath);
        uint getCountOfPipelineSteps();
    private:
        std::vector<std::unique_ptr<PipelineStep>> pipelineSteps;
        void loadPipelineConfig(const std::string& configFilePath);
        void loadPipelineSteps(const json& jsonData);
};

}

#endif // SECOND_TAKE_PIPELINE