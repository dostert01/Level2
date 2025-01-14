#ifndef FILLER_PIPE_H
#define FILLER_PIPE_H

#include <memory>
#include <optional>
#include <shared_mutex>

#include "apistructs.h"

class FillerPipe {
 public:
  FillerPipe() = default;
  virtual ~FillerPipe() = default;
  virtual void addData(string payloadName, string mimetype, string payload);
protected:
  virtual void add2Fifo(shared_ptr<PipelineProcessingData> data) = 0;
};

#endif  // #define FILLER_PIPE_H
