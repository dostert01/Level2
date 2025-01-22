#include "pipelineapi.h"
#include <iostream>
#include <vector>

int pipeline_step_module_init(event_forge::PipelineStepInitData& initData) {
    return 0;
}

int pipeline_step_module_process(event_forge::PipelineProcessingData& processData) {
    std::cout << "Hello World" << std::endl;
    return 0;
}

int pipeline_step_module_finish() {
    return 0;
}