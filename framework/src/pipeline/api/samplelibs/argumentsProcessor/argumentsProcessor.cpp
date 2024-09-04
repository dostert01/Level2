#include "pipelineapi.h"
#include <iostream>

std::string outputFileName;

int pipeline_step_module_init(LibInitData* initData) {

    std::optional<std::string> value = getNamedArgument(initData, "outputFileName");
    if(value.has_value()) {
        outputFileName = value.value();
    }

    return 0;
}

int pipeline_step_module_process(LibProcessingData* processData) {
    std::cout << "Hello World " << outputFileName << std::endl;
    return 0;
}

int pipeline_step_module_finish() {
    return 0;
}