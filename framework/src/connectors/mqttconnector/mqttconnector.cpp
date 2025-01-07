#include <iostream>
#include <fstream>
#include <memory>

#include "pipelineapi.h"
#include "mqttconnector.h"

std::string firstArgument;
std::string secondArgument;

void processTheProcessingData(PipelineProcessingData& processData);
void processArgumentsFromJson();

SpecificBinaryProcessingData::SpecificBinaryProcessingData() : BinaryProcessingData() {}

SpecificBinaryProcessingData::~SpecificBinaryProcessingData() {
  std::cout << "SpecificBinaryProcessingData in deletion has firstArgument: " << firstArgument << std::endl;
  std::cout << "SpecificBinaryProcessingData in deletion has secondArgument: " << secondArgument << std::endl;
}

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
  std::shared_ptr<BinaryProcessingData> binData(new SpecificBinaryProcessingData());
  ((SpecificBinaryProcessingData*)(binData.get()))->firstArgument = firstArgument;
  ((SpecificBinaryProcessingData*)(binData.get()))->secondArgument = secondArgument;
  processData.addPayloadData("myBinaryPayloadData", "", binData);
}

int pipeline_step_module_finish() {
    return 0;
}
