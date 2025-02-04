#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <shared_mutex>

#include "apistructs.h"

namespace level2 {
class PipelineFiFo {
 public:
  PipelineFiFo() = default;
  static std::shared_ptr<PipelineFiFo> getInstance();
  std::optional<std::shared_ptr<PipelineProcessingData>> dequeue();
  void enqueue(std::string payloadName, std::string mimetype, const std::string& payload);
  void enqueue(std::string payloadName, std::string mimetype, const std::string& payload,
               std::map<std::string, std::string> &matchingPatterns);
  void enqueue(std::shared_ptr<PipelineProcessingData> data,
               std::map<std::string, std::string> &matchingPatterns);
  void enqueue(std::shared_ptr<PipelineProcessingData> data);
  int getCountOfElementsInFifo();

 private:
  std::shared_mutex mutex;
  std::queue<std::shared_ptr<PipelineProcessingData>> processingDataFiFo;
  void addMatchingPatterns(std::map<std::string, std::string> &matchingPatterns,
                           std::shared_ptr<PipelineProcessingData> &data);
};

} //namespace level2
