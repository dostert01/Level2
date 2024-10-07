#include "pipelineprocessor.h"
#include "../logger/logger.h"

namespace second_take {

PipeLineProcessor::~PipeLineProcessor() {

}

std::unique_ptr<PipeLineProcessor> PipeLineProcessor::getInstance() {
    std::unique_ptr<PipeLineProcessor> instance = std::make_unique<PipeLineProcessor>();
    return instance;
}

std::optional<std::unique_ptr<PipeLineProcessor>> PipeLineProcessor::getInstance(const std::string configFilePath) {
    std::unique_ptr<PipeLineProcessor> instance = getInstance();
    try {
        instance.get()->loadProcessConfig(configFilePath);
    } catch (const std::exception& e) {
        LOGGER.error("Failed loading process configuration from " + configFilePath + " : " + e.what());
        return std::nullopt;
    }
    return instance;
}

void PipeLineProcessor::loadProcessConfig(const std::string& configFilePath) {

}

uint PipeLineProcessor::getCountOfPipelines() {
    return 0;
}

}