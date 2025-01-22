#include <string.h>

#include "filewriter.h"

using namespace std;

event_forge::FileWriter *fileWriter = nullptr;

int pipeline_step_module_init(event_forge::PipelineStepInitData& initData) {
  fileWriter = new event_forge::FileWriter(initData);
  return 0;
}

int pipeline_step_module_process(event_forge::PipelineProcessingData& processData) {
  fileWriter->writeFile(processData);
  return 0;
}

int pipeline_step_module_finish() {
  if(fileWriter != nullptr)
    delete fileWriter;
  return 0;
}
