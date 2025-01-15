#include "pipelinefifo.h"
#include "logger.h"

using namespace event_forge;

shared_ptr<PipelineFiFo> PipelineFiFo::getInstance() {
    return make_shared<PipelineFiFo>();
}

void PipelineFiFo::enqueue(shared_ptr<PipelineProcessingData> data) {
  map<string, string> emptymatchingPatterns;
  enqueue(data, emptymatchingPatterns);
}

void PipelineFiFo::enqueue(shared_ptr<PipelineProcessingData> data, map<string, string> &matchingPatterns) {
  addMatchingPatterns(matchingPatterns, data);
  unique_lock<shared_mutex> lock(mutex);
  processingDataFiFo.push(data);
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
    map<string, string> &matchingPatterns,
    shared_ptr<PipelineProcessingData> &data) {
  for (const auto &[key, value] : matchingPatterns) {
    data->addMatchingPattern(key, value);
  }
}

optional<shared_ptr<PipelineProcessingData>> PipelineFiFo::dequeue() {
  unique_lock<shared_mutex> lock(mutex);
  if (!processingDataFiFo.empty()) {
    optional<shared_ptr<PipelineProcessingData>> returnValue =
        processingDataFiFo.front();
    processingDataFiFo.pop();
    return returnValue;
  } else {
    return nullopt;
  }
}
