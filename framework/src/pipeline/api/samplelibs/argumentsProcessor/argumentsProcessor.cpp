#include "pipelineapi.h"
#include <iostream>
#include <fstream>

std::string outputFileName;
std::string firstArgument;
std::string secondArgument;

int pipeline_step_module_init(const PipelineStepInitData& initData) {

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

int pipeline_step_module_process(const PipelineProcessingData& processData) {
    std::cout << "pipeline_step_module_process: " << outputFileName << std::endl;
    std::cout << "pipeline_step_module_process: " << firstArgument << std::endl;
    std::cout << "pipeline_step_module_process: " << secondArgument << std::endl;
    std::ofstream writer(outputFileName.c_str());
    writer << firstArgument << std::endl;
    writer << secondArgument << std::endl;
    writer.close();
    return 0;
}

int pipeline_step_module_finish() {
    return 0;
}