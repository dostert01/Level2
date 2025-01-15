#include <iostream>
#include <fstream>
#include <unistd.h>

#include "pipelineprocessor.h"
#include "../logger/logger.h"

namespace event_forge {

#define JSON_PROPERTY_PIPELINES "pipelines"
#define JSON_PROPERTY_PIPELINE_CONFIG_FILE "pipelineConfigFile"
#define JSON_PROPERTY_PROCESS_NAME "processName"
#define JSON_PROPERTY_MATCHING_PATTERNS_ARRAY "matchingPatterns"

#define HALF_A_SECOND 500000
#define DIR_SEPARATOR "/"

PipeLineProcessor::~PipeLineProcessor() {
    LOGGER.info("Unloading the PipeLineProcessor");
    stopProcessingLoop();
}

std::unique_ptr<PipeLineProcessor> PipeLineProcessor::getInstance() {
    std::unique_ptr<PipeLineProcessor> instance = std::make_unique<PipeLineProcessor>();
    return instance;
}

std::optional<std::unique_ptr<PipeLineProcessor>> PipeLineProcessor::getInstance(const std::string configFilePath) {
    std::unique_ptr<PipeLineProcessor> instance = getInstance();
    try {
        LOGGER.info("Loading pipeline processor configuration from file: " + configFilePath);
        instance.get()->loadProcessorConfig(configFilePath);
    } catch (const std::exception& e) {
        LOGGER.error("Failed loading pipeline processor configuration from " + configFilePath + " : " + e.what());
        return std::nullopt;
    }
    return instance;
}

void PipeLineProcessor::loadProcessorConfig(const std::string& configFilePath) {
  setConfigFileDirFromConfigFileName(configFilePath);
  try {
    std::ifstream jsonFile(configFilePath);
    json jsonData = json::parse(jsonFile);
    jsonFile.close();
    loadHeaderData(jsonData);
    loadPipelines(jsonData);
    } catch (const std::exception& e) {
        throw;
    }
}

void PipeLineProcessor::setConfigFileDirFromConfigFileName(
    const std::string& configFilePath) {
  configFileDir = getDirNameFromPath(configFilePath);
  LOGGER.info("Reading config files from directory: '" + configFileDir + "'");
}

std::string PipeLineProcessor::getDirNameFromPath(const std::string path) {
    size_t lastSlashPos = path.find_last_of(DIR_SEPARATOR);
    return (lastSlashPos == std::string::npos) ? "" : path.substr(0, lastSlashPos);
}

void PipeLineProcessor::loadHeaderData(const json& jsonData) {
    if(jsonData.contains(JSON_PROPERTY_PROCESS_NAME) && jsonData[JSON_PROPERTY_PROCESS_NAME].is_string()) {
        processName = jsonData[JSON_PROPERTY_PROCESS_NAME];
        LOGGER.info("Setting processName to: " + processName);
    } else {
        LOGGER.warn("Property '" + std::string(JSON_PROPERTY_PROCESS_NAME ) +
            "' not found while loading the process definition!");
    }
}

void PipeLineProcessor::loadPipelines(const json& jsonData) {
    if(jsonData.contains(JSON_PROPERTY_PIPELINES) && jsonData[JSON_PROPERTY_PIPELINES].is_array()) {
        for (const auto& currentPipelineDefinition : jsonData[JSON_PROPERTY_PIPELINES]) {
            std::optional<std::shared_ptr<Pipeline>> currentPipeline =
                Pipeline::getInstance(getPipelineConfigFileNameFromJsonData(currentPipelineDefinition));
            if(currentPipeline.has_value()) {
                readMatchingPatternsFromJson(currentPipeline.value(), currentPipelineDefinition);
                pipelines.push_back(std::move(currentPipeline.value()));
            } else {
                throw PipelineException("Failed to load pipeline from file: " +
                    getPipelineConfigFileNameFromJsonData(currentPipelineDefinition));
            }
        }
    } else {
        throw PipelineException("Failed to load pipelines: No pipelines array found in json.");
    }
}

void PipeLineProcessor::readMatchingPatternsFromJson(std::shared_ptr<Pipeline> pipeline, const json& pipelineDefinition) {
    if(pipelineDefinition.contains(JSON_PROPERTY_MATCHING_PATTERNS_ARRAY) && pipelineDefinition[JSON_PROPERTY_MATCHING_PATTERNS_ARRAY].is_array()) {
        LOGGER.debug("Reading " + std::string(JSON_PROPERTY_MATCHING_PATTERNS_ARRAY) + " for pipeline");
        for(auto& pattern : pipelineDefinition[JSON_PROPERTY_MATCHING_PATTERNS_ARRAY].items()) {
            for(auto& object : pattern.value().items()) {
                LOGGER.debug(object.key() + " = " + object.value().get<std::string>());
                pipeline.get()->addMatchingPattern(object.key(), object.value().get<std::string>());
            }
        }
    }
}

std::string PipeLineProcessor::getPipelineConfigFileNameFromJsonData(const nlohmann::json_abi_v3_11_3::json &currentPipelineDefinition) {
    std::string currentPipelineConfigFileName = currentPipelineDefinition.value(JSON_PROPERTY_PIPELINE_CONFIG_FILE, UNDEFINED_JSON_DATA);
    if(!configFileDir.empty()) {
        currentPipelineConfigFileName = configFileDir + DIR_SEPARATOR + currentPipelineConfigFileName;
    }
    LOGGER.info("Loading pipeline from config file: " + currentPipelineConfigFileName);
    return currentPipelineConfigFileName;
}

std::optional<std::shared_ptr<Pipeline>> PipeLineProcessor::getPipelineByName(std::string pipelineName) {
    for(const auto& currentPipeline : pipelines) {
        if(currentPipeline.get()->getPipelineName() == pipelineName)
            return currentPipeline;
    }
    return std::nullopt;
}

uint PipeLineProcessor::getCountOfPipelines() {
    return pipelines.size();
}

std::string PipeLineProcessor::getProcessName() {
    return processName;
}

void PipeLineProcessor::execute(PipelineProcessingData& payload) {
    std::unique_lock<std::shared_mutex> lock(pipelineExecutionMutex);
    for(const auto& currentPipeline : pipelines) {
        currentPipeline.get()->execute(payload);
    }
}

void PipeLineProcessor::processingLoop(shared_ptr<PipelineFiFo> fifo) {
    LOGGER.debug("processing loop started");
    int sleepTime = HALF_A_SECOND;
    while(keepThreadRunning) {
        optional<shared_ptr<PipelineProcessingData>> data =
            fifo->dequeue();
        if(data.has_value()) {
            sleepTime = 1;
            //execute(*data.value());
        } else {
            if(sleepTime < HALF_A_SECOND) {
                sleepTime *= 2;
            }
        }
        usleep(sleepTime);
    }
    LOGGER.debug("processing loop ending");
}

void PipeLineProcessor::startProcessingLoop(shared_ptr<PipelineFiFo> fifo) {
    LOGGER.debug("starting the processing loop");
    keepThreadRunning = true;
    processingLoopThread = std::thread(
        &PipeLineProcessor::processingLoop, this, fifo);
}

bool PipeLineProcessor::isProcessingLoopRunning() {
    return processingLoopThread.joinable();
}

void PipeLineProcessor::stopProcessingLoop() {
    keepThreadRunning = false;
    if(processingLoopThread.joinable()) {
        LOGGER.debug("stopping the processing loop");
        processingLoopThread.join();
    }
}


}