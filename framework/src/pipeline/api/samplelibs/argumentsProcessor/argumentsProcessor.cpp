#include "pipelineapi.h"
#include <iostream>
#include <fstream>

std::string outputFileName;
std::string firstArgument;
std::string secondArgument;

int pipeline_step_module_init(PipelineStepInitData& initData) {

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

int pipeline_step_module_process(PipelineProcessingData& processData) {
  processArgumentsFromJson();
  processTheProcessingData(processData);
  return 0;
}

void processTheProcessingData(PipelineProcessingData& processData) {
  std::optional<ProcessingPayload*> p = processData.getPayload("question");
  if ((p.has_value()) &&
      (p.value()->payloadAsString().compare("What is the answer?") == 0)) {
    std::cout << "adding the answer to the question" << std::endl;
    processData.addPayloadData("answer", "text/plain", "the answer is 42");
  } else {
    std::cout << "NOT adding the answer to the question: " << p.value()->payloadAsString() << std::endl;
  }
}

void processArgumentsFromJson() {
  std::cout << "pipeline_step_module_process: " << outputFileName << std::endl;
  std::cout << "pipeline_step_module_process: " << firstArgument << std::endl;
  std::cout << "pipeline_step_module_process: " << secondArgument << std::endl;
  std::ofstream writer(outputFileName.c_str());
  writer << firstArgument << std::endl;
  writer << secondArgument << std::endl;
  writer.close();
}

int pipeline_step_module_finish() {
    return 0;
}