#include <iostream>
#include <fstream>

#include "pipelineapi.h"

std::string outputFileName;
std::string firstArgument;
std::string secondArgument;
std::string error01;
std::string error02;

void processTheProcessingData(PipelineProcessingData& processData);
void processArgumentsFromJson();

int pipeline_step_module_init(PipelineStepInitData& initData) {

    std::optional<std::string> s = initData.getNamedArgument("outputFileName");
    if(s.has_value()) {
        outputFileName = s.value();
    }
    s = initData.getNamedArgument("first argument");
    if(s.has_value()) {
        firstArgument = s.value();
    }
    s = initData.getNamedArgument("second argument");
    if(s.has_value()) {
        secondArgument = s.value();
    }
    s = initData.getNamedArgument("raise Error 01");
    if(s.has_value()) {
        error01 = s.value();
    }
    s = initData.getNamedArgument("raise Error 02");
    if(s.has_value()) {
        error02 = s.value();
    }
    return 0;
}

int pipeline_step_module_process(PipelineProcessingData& processData) {
  processArgumentsFromJson();
  processTheProcessingData(processData);
  return 0;
}

void processTheProcessingData(PipelineProcessingData& processData) {
  std::optional<std::shared_ptr<ProcessingPayload>> p = processData.getPayload("question");
  if ((p.has_value()) &&
      (p.value().get()->payloadAsString().compare("What is the answer?") == 0)) {
    std::cout << "adding the answer to the question" << std::endl;
    processData.addPayloadData("answer", "text/plain", "the answer is 42");
  } else {
    std::cout << "NOT adding the answer to the question" << std::endl;
  }

  if(!error01.empty())
    processData.addError("error 01", error01);
  if(!error02.empty())
    processData.addError("error 02", error02);
}

void processArgumentsFromJson() {
  if(!outputFileName.empty()) {
    std::cout << "pipeline_step_module_process: " << outputFileName << std::endl;
    std::cout << "pipeline_step_module_process: " << firstArgument << std::endl;
    std::cout << "pipeline_step_module_process: " << secondArgument << std::endl;
    std::ofstream writer(outputFileName.c_str());
    writer << firstArgument << std::endl;
    writer << secondArgument << std::endl;
    writer.close();
  }
}

int pipeline_step_module_finish() {
    return 0;
}