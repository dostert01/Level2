#include <string.h>

#include "pipelineapi.h"
#include "logger.h"

#define PARAM_NAME_TARGET_DIRECTORY "targetDirectory"

using namespace std;

string targetDirectory;

string getNamedArgument(PipelineStepInitData& initData, string paramName) {
  string value = initData.getNamedArgument(paramName).value_or("");
  LOGGER.info(paramName + ": " + value);
  return value;
}

void init(PipelineStepInitData& initData) {
  string hostName = getNamedArgument(initData, PARAM_NAME_TARGET_DIRECTORY);

  if(targetDirectory.empty()) {
    LOGGER.error("Failed loading init parameter for " +
      string(PARAM_NAME_TARGET_DIRECTORY) + ". Storing of files will stay disabled!");
  } else {
    LOGGER.info("file connector configured to write files to: " + targetDirectory);
  }
}

int pipeline_step_module_init(PipelineStepInitData& initData) {
  init(initData);
  return 0;
}

void sendData(PipelineProcessingData& processData) {
}

int pipeline_step_module_process(PipelineProcessingData& processData) {
  sendData(processData);
  return 0;
}

int pipeline_step_module_finish() {
  return 0;
}

