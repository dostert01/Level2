#include <memory>
#include <iostream>
#include <fstream>
#include <dlfcn.h>

#include "../logger/logger.h"
#include "pipeline.h"

namespace second_take {

#define UNDEFINED_JSON_DATA "undefined"
#define JSON_PROPERTY_PIPELINE_NAME "pipelineName"
#define JSON_PROPERTY_PIPELINE_STEPS "pipelineSteps"
#define JSON_PROPERTY_STEP_NAME "stepName"
#define JSON_PROPERTY_LIBRARY_NAME "libraryName"
#define JSON_PROPERTY_NAMED_ARGUMENTS "namedArguments"

std::unique_ptr<Pipeline> Pipeline::getInstance() {
    std::unique_ptr<Pipeline> instance = std::make_unique<Pipeline>();
    return instance;
}

std::optional<std::unique_ptr<Pipeline>> Pipeline::getInstance(const std::string configFilePath) {
    std::unique_ptr<Pipeline> instance = getInstance();
    try {
        instance.get()->loadPipelineConfig(configFilePath);
    } catch (const std::exception& e) {
        second_take::Logger::getInstance().error("Failed loading pipeline configuration from " + configFilePath + " : " + e.what());
        return std::nullopt;
    }
    return instance;
}

uint Pipeline::getCountOfPipelineSteps() {
    return pipelineSteps.size();
}

void Pipeline::execute() {
    for(const auto& currentStep : pipelineSteps) {
        currentStep.get()->getProcessFunction()(NULL);
    }
}

std::optional<PipelineStep*> Pipeline::getStepByName(const std::string& stepName) {
    for(const auto& currentStep : pipelineSteps) {
        if(currentStep.get()->getStepName() == stepName){
            return currentStep.get();
        }
    }
    return nullptr;
}

std::string Pipeline::getPipelineName() {
    return pipelineName;
}

void Pipeline::setPipelineName(const std::string& pipelineName) {
    this->pipelineName = pipelineName;
}

void Pipeline::loadPipelineConfig(const std::string& configFilePath) {
    try {
        std::ifstream jsonFile(configFilePath);
        json jsonData = json::parse(jsonFile);
        jsonFile.close();
        loadPipelineMetaData(jsonData);
        loadPipelineSteps(jsonData);
    } catch (const std::exception& e) {
        throw;
    }
}

void Pipeline::loadPipelineMetaData(const json& jsonData) {
    if(jsonData.contains(JSON_PROPERTY_PIPELINE_NAME)) {
        setPipelineName(jsonData.value(JSON_PROPERTY_PIPELINE_NAME, UNDEFINED_JSON_DATA));
    } else {
        throw PipelineException("Failed to load pipeline metadata. pipelineName not set");
    }
}

void Pipeline::loadPipelineSteps(const json& jsonData) {
    if(jsonData.contains(JSON_PROPERTY_PIPELINE_STEPS) && jsonData[JSON_PROPERTY_PIPELINE_STEPS].is_array()) {
        for (const auto& step : jsonData[JSON_PROPERTY_PIPELINE_STEPS]) {
            std::unique_ptr<PipelineStep> currentStep = std::make_unique<PipelineStep>();
            currentStep.get()->setStepName(step.value(JSON_PROPERTY_STEP_NAME, UNDEFINED_JSON_DATA));
            currentStep.get()->setLibraryName(step.value(JSON_PROPERTY_LIBRARY_NAME, UNDEFINED_JSON_DATA));
            currentStep.get()->loadNamedArguments(step);
            try {
                currentStep.get()->loadLib();
            } catch (const std::exception& e) {
                throw;
            }
            if(currentStep.get()->isInitComplete()) {
                pipelineSteps.push_back(std::move(currentStep));
            } else {
                throw PipelineException("Failed to load pipeline step: Definition of pipeline steps in json might be incomplete.");
            }
        }
    } else {
        throw PipelineException("Failed to load pipeline step: No pipeline steps array found in json.");
    }
}

//-------------------------------------------------------------------
PipelineStep::~PipelineStep() {
    if(hLib != NULL) {
        dlclose(hLib);
        hLib = NULL;
    }
}

void PipelineStep::loadNamedArguments(const json& jsonData) {
    if(jsonData.contains(JSON_PROPERTY_NAMED_ARGUMENTS) && jsonData[JSON_PROPERTY_NAMED_ARGUMENTS].is_object()) {
        for (const auto& item : jsonData[JSON_PROPERTY_NAMED_ARGUMENTS].items()) {
            initData.namedArguments[item.key()] = item.value().get<std::string>();
        }
    }
}

void PipelineStep::setStepName(const std::string stepName) {
    this->stepName = stepName;
}

std::string PipelineStep::getStepName() {
    return stepName;
}

void PipelineStep::setLibraryName(const std::string libraryName) {
    this->libraryName = libraryName;
}

void PipelineStep::loadLib() {
    std::string libraryFileName = "lib" + libraryName + ".so";
    hLib = dlopen(libraryFileName.c_str(), RTLD_LAZY);
    if(hLib == NULL) {
        throw PipelineException("Failed to load pipeline step from shared object " + libraryFileName + " : " + dlerror());
    }
    libInit = (LibInitFunction)(dlsym(hLib, "pipeline_step_module_init"));
    if(libInit == NULL) {
        throw PipelineException("Failed to load pipeline step from shared object " + libraryFileName + " while trying to load pipeline_step_module_init : "  + dlerror());
    }
    libProcess = (LibProcessFunction)(dlsym(hLib, "pipeline_step_module_process"));
    if(libProcess == NULL) {
        throw PipelineException("Failed to load pipeline step from shared object " + libraryFileName + " while trying to load pipeline_step_module_process : "  + dlerror());
    }
    libFinish = (LibFinishFunction)(dlsym(hLib, "pipeline_step_module_finish"));
    if(libFinish == NULL) {
        throw PipelineException("Failed to load pipeline step from shared object " + libraryFileName + " while trying to load pipeline_step_module_finish : "  + dlerror());
    }
    getInitFunction()(&initData);
}

bool PipelineStep::isInitComplete() {
    return (stepName != UNDEFINED_JSON_DATA) &&
        (libraryName != UNDEFINED_JSON_DATA) &&
        (hLib != NULL) &&
        (libInit != NULL) &&
        (libProcess != NULL) &&
        (libFinish != NULL);
}

LibInitFunction PipelineStep::getInitFunction() const {
    return libInit;
};

LibProcessFunction PipelineStep::getProcessFunction() const {
    return libProcess;
}

LibFinishFunction PipelineStep::getFinishFunction() const {
    return libFinish;
}

//-------------------------------------------------------------------
PipelineException::PipelineException(const std::string& message) : std::exception() {
    this->message = message;
}

const char* PipelineException::what() const noexcept {
    return (char*)message.c_str();
}



}