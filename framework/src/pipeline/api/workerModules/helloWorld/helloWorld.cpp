#include "pipelineapi.h"
#include <iostream>
#include <vector>

int pipeline_step_module_init(level2::PipelineStepInitData& initData) {
    return 0;
}

int pipeline_step_module_process(level2::PipelineProcessingData& processData) {
    std::cout << "Hello World" << std::endl;
    processData.addPayloadData("data to be sent back to the http client", "text/plain", "hello world");
    processData.addMatchingPattern("processed_by_hello_world", "true");
    if(processData.getMatchingPattern("http.rcv.urlParameter.what").value_or("") == "throwError") {
        processData.addError("-42", "this error has been thrown on purpose");
    }
    return 0;
}

int pipeline_step_module_finish() {
    return 0;
}