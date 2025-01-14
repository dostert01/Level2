#include "pipelinefifo.h"
#include "logger.h"

using namespace event_forge;

shared_ptr<PipelineFiFo> PipelineFiFo::getInstance() {
    return make_shared<PipelineFiFo>();
}

void PipelineFiFo::enqueue(shared_ptr<PipelineProcessingData> data, map<string, string> &matchingPatterns) {
  addMatchingPatterns(matchingPatterns, data);
  std::unique_lock<std::shared_mutex> lock(mutex);
  processingDataFiFo.push(data);
}

optional<shared_ptr<PipelineProcessingData>> PipelineFiFo::dequeue() {
  std::unique_lock<std::shared_mutex> lock(mutex);
  if (!processingDataFiFo.empty()) {
    optional<shared_ptr<PipelineProcessingData>> returnValue =
        processingDataFiFo.front();
    processingDataFiFo.pop();
    return returnValue;
  } else {
    return nullopt;
  }
}

void PipelineFiFo::enqueue(string payloadName, string mimetype, string payload) {
    map<string, string> emptyMatchingPatterns;
    enqueue(payloadName, mimetype, payload, emptyMatchingPatterns);
}

void PipelineFiFo::enqueue(string payloadName, string mimetype, string payload, map<string, string> &matchingPatterns) {
    shared_ptr<PipelineProcessingData> data = make_shared<PipelineProcessingData>();
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
