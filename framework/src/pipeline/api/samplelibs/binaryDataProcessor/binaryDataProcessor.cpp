#include <iostream>
#include <fstream>
#include <memory>

#include "pipelineapi.h"
#include "binaryDataProcessor.h"

std::string firstArgument;
std::string secondArgument;

void processTheProcessingData(event_forge::PipelineProcessingData& processData);
void processArgumentsFromJson();

int pipeline_step_module_init(event_forge::PipelineStepInitData& initData) {

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

int pipeline_step_module_process(event_forge::PipelineProcessingData& processData) {
  processTheProcessingData(processData);
  return 0;
}

void processTheProcessingData(event_forge::PipelineProcessingData& processData) {
  auto binData = make_shared<event_forge::ProcessingError>("ProcessingError",
    "ProcessingError is the only BinaryDataPayload, that currently is supported. "
    "firstArgument: " + firstArgument + " " +
    "secondArgument: " + secondArgument);
  processData.addPayloadData("myBinaryPayloadData", "", binData);
}

int pipeline_step_module_finish() {
    return 0;
}
