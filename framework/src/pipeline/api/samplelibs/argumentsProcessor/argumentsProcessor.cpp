#include "pipelineapi.h"
#include <iostream>

std::string outputFileName;
std::string firstArgument;
std::string secondArgument;

int pipeline_step_module_init(LibInitData& initData) {

    std::optional<std::string> s = getNamedArgument(initData, "outputFileName");
    if(s.has_value()) {
        outputFileName = s.value();
    }
    s = getNamedArgument(initData, "first argument");
    if(s.has_value()) {
        firstArgument = s.value();
    }
    s = getNamedArgument(initData, "second argument");
    if(s.has_value()) {
        secondArgument = s.value();
    }
    return 0;
}

int pipeline_step_module_process(LibProcessingData& processData) {
    std::cout << "pipeline_step_module_process: " << outputFileName << std::endl;
    std::cout << "pipeline_step_module_process: " << firstArgument << std::endl;
    std::cout << "pipeline_step_module_process: " << secondArgument << std::endl;
    return 0;
}

int pipeline_step_module_finish() {
    return 0;
}