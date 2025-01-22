#include "pipelinefifo.h"
#include "logger.h"

using namespace event_forge;

namespace event_forge {

std::shared_ptr<PipelineFiFo> PipelineFiFo::getInstance() {
    return std::make_shared<PipelineFiFo>();
}

void PipelineFiFo::enqueue(std::shared_ptr<PipelineProcessingData> data) {
  std::map<std::string, std::string> emptymatchingPatterns;
  enqueue(data, emptymatchingPatterns);
}

void PipelineFiFo::enqueue(std::shared_ptr<PipelineProcessingData> data, std::map<std::string, std::string> &matchingPatterns) {
  addMatchingPatterns(matchingPatterns, data);
  std::unique_lock<std::shared_mutex> lock(mutex);
  processingDataFiFo.push(data);
}

void PipelineFiFo::enqueue(std::string payloadName, std::string mimetype, const std::string& payload) {
    std::map<std::string, std::string> emptyMatchingPatterns;
    enqueue(payloadName, mimetype, payload, emptyMatchingPatterns);
}

void PipelineFiFo::enqueue(std::string payloadName, std::string mimetype, const std::string& payload, std::map<std::string, std::string> &matchingPatterns) {
    std::shared_ptr<PipelineProcessingData> data = std::make_shared<PipelineProcessingData>();
    data->addPayloadData(payloadName, mimetype, payload);
    enqueue(data, matchingPatterns);
}

void PipelineFiFo::addMatchingPatterns(
    std::map<std::string, std::string> &matchingPatterns,
    std::shared_ptr<PipelineProcessingData> &data) {
  for (const auto &[key, value] : matchingPatterns) {
    data->addMatchingPattern(key, value);
  }
}

std::optional<std::shared_ptr<PipelineProcessingData>> PipelineFiFo::dequeue() {
  std::unique_lock<std::shared_mutex> lock(mutex);
  if (!processingDataFiFo.empty()) {
    std::optional<std::shared_ptr<PipelineProcessingData>> returnValue =
        processingDataFiFo.front();
    processingDataFiFo.pop();
    return returnValue;
  } else {
    return std::nullopt;
  }
}

int PipelineFiFo::getCountOfElementsInFifo() {
  std::unique_lock<std::shared_mutex> lock(mutex);
  return processingDataFiFo.size();
}
} // namespace event_forge