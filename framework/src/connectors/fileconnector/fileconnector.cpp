#include <string.h>

#include "filewriter.h"

using namespace std;

level2::FileWriter *fileWriter = nullptr;

int pipeline_step_module_init(level2::PipelineStepInitData& initData) {
  fileWriter = new level2::FileWriter(initData);
  return 0;
}

int pipeline_step_module_process(level2::PipelineProcessingData& processData) {
  fileWriter->writeFile(processData);
  return 0;
}

int pipeline_step_module_finish() {
  if(fileWriter != nullptr)
    delete fileWriter;
  return 0;
}
