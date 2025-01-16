#include <memory>
#include <iostream>
#include <fstream>
#include <dlfcn.h>

#include "logger.h"
#include "pipeline.h"

namespace event_forge {

#define JSON_PROPERTY_PIPELINE_NAME "pipelineName"
#define JSON_PROPERTY_PIPELINE_STEPS "pipelineSteps"
#define JSON_PROPERTY_STEP_NAME "stepName"
#define JSON_PROPERTY_LIBRARY_NAME "libraryName"
#define JSON_PROPERTY_NAMED_ARGUMENTS "namedArguments"

shared_ptr<Pipeline> Pipeline::getInstance() {
    shared_ptr<Pipeline> instance = make_shared<Pipeline>();
    return instance;
}

optional<shared_ptr<Pipeline>> Pipeline::getInstance(const string configFilePath) {
    auto instance = getInstance();
    try {
        instance.get()->loadPipelineConfig(configFilePath);
    } catch (const exception& e) {
        LOGGER.error("Failed loading pipeline configuration from " + configFilePath + " : " + e.what());
        return nullopt;
    }
    return instance;
}

uint Pipeline::getCountOfPipelineSteps() {
    return pipelineSteps.size();
}

void Pipeline::execute() {
    auto emptyProcessingData = make_shared<PipelineProcessingData>();
    execute(emptyProcessingData);
}

void Pipeline::execute(shared_ptr<PipelineProcessingData> processData) {
    if((!hasMatchingPatterns() && !processData->hasMatchingPatterns()) || matchesAllOfMineToAnyOfTheOther(processData)) {
        LOGGER.info("Start processing payload by pipeline '" + getPipelineName() + "'");
        tagProcessingData(processData);
        for(const auto& currentStep : pipelineSteps) {
            currentStep.get()->runProcessingFunction(processData);
        }
    } else {
        LOGGER.info("Pipeline '" + getPipelineName() + "' rejects processing of payload because of non matching patterns.");
    }
}

void Pipeline::tagProcessingData(shared_ptr<PipelineProcessingData> processData) {
    processData->increaseProcessingCounter();
    processData->setLastProcessedPipelineName(getPipelineName());
}

optional<shared_ptr<PipelineStep>> Pipeline::getStepByName(const string& stepName) {
    for(const auto& currentStep : pipelineSteps) {
        if(currentStep.get()->getStepName() == stepName){
            return currentStep;
        }
    }
    return nullptr;
}

string Pipeline::getPipelineName() {
    return pipelineName;
}

void Pipeline::setPipelineName(const string& pipelineName) {
    this->pipelineName = pipelineName;
}

void Pipeline::loadPipelineConfig(const string& configFilePath) {
    try {
        ifstream jsonFile(configFilePath);
        json jsonData = json::parse(jsonFile);
        jsonFile.close();
        loadPipelineMetaData(jsonData);
        loadPipelineSteps(jsonData);
    } catch (const exception& e) {
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
            shared_ptr<PipelineStep> currentStep = make_shared<PipelineStep>();
            currentStep.get()->setStepName(step.value(JSON_PROPERTY_STEP_NAME, UNDEFINED_JSON_DATA));
            currentStep.get()->setLibraryName(step.value(JSON_PROPERTY_LIBRARY_NAME, UNDEFINED_JSON_DATA));
            currentStep.get()->loadNamedArguments(step);
            try {
                currentStep.get()->loadLib();
            } catch (const exception& e) {
                throw;
            }
            if(currentStep.get()->isInitComplete()) {
                pipelineSteps.push_back(currentStep);
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
        runFinishFunction();
        LOGGER.info("Unloading shared object from pipeline: " + this->libraryName);
        if(dlclose(hLib) == 0) {
            hLib = NULL;
        } else {
            LOGGER.error("Error during unloading of shared object from pipeline: " + this->libraryName + " : " + dlerror());
        }
    }
}

void PipelineStep::loadNamedArguments(const json& jsonData) {
    if(jsonData.contains(JSON_PROPERTY_NAMED_ARGUMENTS) && jsonData[JSON_PROPERTY_NAMED_ARGUMENTS].is_object()) {
        for (const auto& item : jsonData[JSON_PROPERTY_NAMED_ARGUMENTS].items()) {
            initData.namedArguments.insert({item.key(), (item.value().get<string>())});
        }
    }
}

void PipelineStep::setStepName(const string stepName) {
    this->stepName = stepName;
}

string PipelineStep::getStepName() {
    return stepName;
}

void PipelineStep::setLibraryName(const string libraryName) {
    this->libraryName = libraryName;
}

void PipelineStep::loadLib() {
    string libraryFileName = "lib" + libraryName + ".so";
    LOGGER.info("Loading shared object into pipeline: " + libraryFileName);
    hLib = dlopen(libraryFileName.c_str(), RTLD_LAZY);
    char* error = dlerror();
    if((hLib == NULL) && (error != NULL)) {
        throw PipelineException("Failed to load pipeline step from shared object " + libraryFileName + " : " + error);
    } else if((hLib != NULL) && (error != NULL)) {
        throw PipelineException("Shared object " + libraryFileName + " loaded into pipeline with error : " + error);
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
    runInitFunction();
}

bool PipelineStep::isInitComplete() {
    return (stepName != UNDEFINED_JSON_DATA) &&
        (libraryName != UNDEFINED_JSON_DATA) &&
        (hLib != NULL) &&
        (libInit != NULL) &&
        (libProcess != NULL) &&
        (libFinish != NULL);
}

void PipelineStep::runInitFunction() {
    libInit(initData);
}

void PipelineStep::runProcessingFunction(shared_ptr<PipelineProcessingData> processingData) { 
    libProcess(*processingData);
}

void PipelineStep::runFinishFunction() {
    LOGGER.info("Invoking the finish function for: " + this->libraryName);
    libFinish();
}

//-------------------------------------------------------------------
PipelineException::PipelineException(const string& message) : exception() {
    this->message = message;
}

const char* PipelineException::what() const noexcept {
    return (char*)message.c_str();
}



}