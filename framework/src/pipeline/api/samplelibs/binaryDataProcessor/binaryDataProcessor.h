#ifndef BINARY_DATA_PROCESSOR_H
#define BINARY_DATA_PROCESSOR_H

#include "pipelineapi.h"
#include <string>

class SpecificBinaryProcessingData : public BinaryProcessingData {
  public:
    std::string firstArgument;
    std::string secondArgument;
    SpecificBinaryProcessingData();
    ~SpecificBinaryProcessingData();
};


#endif //BINARY_DATA_PROCESSOR_H