#include "pipelineprocessor.h"
#include "../logger/logger.h"

namespace second_take {

Process::~Process() {

}

std::unique_ptr<Process> Process::getInstance() {
    std::unique_ptr<Process> instance = std::make_unique<Process>();
    return instance;
}

std::optional<std::unique_ptr<Process>> Process::getInstance(const std::string configFilePath) {
    std::unique_ptr<Process> instance = getInstance();
    try {
        instance.get()->loadProcessConfig(configFilePath);
    } catch (const std::exception& e) {
        LOGGER.error("Failed loading process configuration from " + configFilePath + " : " + e.what());
        return std::nullopt;
    }
    return instance;
}

void Process::loadProcessConfig(const std::string& configFilePath) {

}

uint Process::getCountOfPipelines() {
    return 0;
}

}