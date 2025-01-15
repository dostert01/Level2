#ifndef PIPELINE_FIFO_H
#define PIPELINE_FIFO_H

#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <shared_mutex>

#include "apistructs.h"

using namespace std;

class PipelineFiFo {
 public:
  PipelineFiFo() = default;
  static shared_ptr<PipelineFiFo> getInstance();
  optional<shared_ptr<PipelineProcessingData>> dequeue();
  void enqueue(string payloadName, string mimetype, string payload);
  void enqueue(string payloadName, string mimetype, string payload,
               map<string, string> &matchingPatterns);
  void enqueue(shared_ptr<PipelineProcessingData> data,
               map<string, string> &matchingPatterns);
  void enqueue(shared_ptr<PipelineProcessingData> data);


 private:
  std::shared_mutex mutex;
  queue<shared_ptr<PipelineProcessingData>> processingDataFiFo;
  void addMatchingPatterns(std::map<std::string, std::string> &matchingPatterns,
                           std::shared_ptr<PipelineProcessingData> &data);
};

#endif  // #ifndef PIPELINE_FIFO_H