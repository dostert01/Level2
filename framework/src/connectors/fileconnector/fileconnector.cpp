#include <string.h>

#include "filewriter.h"



using namespace std;

FileWriter *fileWriter = nullptr;

int pipeline_step_module_init(PipelineStepInitData& initData) {
  fileWriter = new FileWriter(initData);
  return 0;
}

int pipeline_step_module_process(PipelineProcessingData& processData) {
  fileWriter->writeFile(processData);
  return 0;
}

int pipeline_step_module_finish() {
  if(fileWriter != nullptr)
    delete fileWriter;
  return 0;
}
