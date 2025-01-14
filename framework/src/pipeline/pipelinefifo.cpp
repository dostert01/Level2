#include "pipelinefifo.h"
#include "logger.h"

using namespace event_forge;

shared_ptr<PipelineFiFo> PipelineFiFo::getInstance() {
    return make_shared<PipelineFiFo>();
}

void PipelineFiFo::enqueue(shared_ptr<PipelineProcessingData> data) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    processingDataFiFo.push(data);
}

optional<shared_ptr<PipelineProcessingData>> PipelineFiFo::dequeue() {
    std::unique_lock<std::shared_mutex> lock(mutex);
    if(!processingDataFiFo.empty()) {
        optional<shared_ptr<PipelineProcessingData>> returnValue = processingDataFiFo.front();
        processingDataFiFo.pop();
        return returnValue;
    } else {
        return nullopt;
    }
}

void PipelineFiFo::enqueue(string payloadName, string mimetype, string payload) {
    shared_ptr<PipelineProcessingData> data = make_shared<PipelineProcessingData>();
    data->addPayloadData(payloadName, mimetype, payload);
    enqueue(data);
}
