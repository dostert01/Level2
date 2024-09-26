#include "pipelineapi.h"
#include <iostream>
#include <vector>
int pipeline_step_module_init(PipelineStepInitData& initData) {
    return 0;
}

int pipeline_step_module_process(PipelineProcessingData& processData) {
    std::cout << "Hello World" << std::endl;
    return 0;
}

int pipeline_step_module_finish() {
    return 0;
}