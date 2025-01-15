#include <iostream>
#include <fstream>
#include <memory>

#include "pipelineapi.h"
#include "binaryDataProcessor.h"

std::string firstArgument;
std::string secondArgument;

void processTheProcessingData(PipelineProcessingData& processData);
void processArgumentsFromJson();

int pipeline_step_module_init(PipelineStepInitData& initData) {

    std::optional<std::string> s  = initData.getNamedArgument("first argument");
    if(s.has_value()) {
        firstArgument = s.value();
    }
    s = initData.getNamedArgument("second argument");
    if(s.has_value()) {
        secondArgument = s.value();
    }
    return 0;
}

int pipeline_step_module_process(PipelineProcessingData& processData) {
  processTheProcessingData(processData);
  return 0;
}

void processTheProcessingData(PipelineProcessingData& processData) {
  auto binData = make_shared<ProcessingError>("ProcessingError",
    "ProcessingError is the only BinaryDataPayload, that currently is supported. "
    "firstArgument: " + firstArgument + " " +
    "secondArgument: " + secondArgument);
  processData.addPayloadData("myBinaryPayloadData", "", binData);
}

int pipeline_step_module_finish() {
    return 0;
}
