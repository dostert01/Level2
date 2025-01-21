#ifndef PIPELINE_FIFO_H
#define PIPELINE_FIFO_H

#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <shared_mutex>

#include "apistructs.h"

using namespace std;

namespace event_forge {
class PipelineFiFo {
 public:
  PipelineFiFo() = default;
  static shared_ptr<PipelineFiFo> getInstance();
  optional<shared_ptr<PipelineProcessingData>> dequeue();
  void enqueue(string payloadName, string mimetype, const string& payload);
  void enqueue(string payloadName, string mimetype, const string& payload,
               map<string, string> &matchingPatterns);
  void enqueue(shared_ptr<PipelineProcessingData> data,
               map<string, string> &matchingPatterns);
  void enqueue(shared_ptr<PipelineProcessingData> data);
  int getCountOfElementsInFifo();

 private:
  shared_mutex mutex;
  queue<shared_ptr<PipelineProcessingData>> processingDataFiFo;
  void addMatchingPatterns(map<string, string> &matchingPatterns,
                           shared_ptr<PipelineProcessingData> &data);
};

} //namespace event_forge
#endif  // #ifndef PIPELINE_FIFO_H