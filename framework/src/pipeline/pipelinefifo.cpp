#include "pipelinefifo.h"
#include "logger.h"

namespace event_forge {

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

void PipelineFiFo::fillIntoPipeline4AsynchronousProcessing(shared_ptr<PipelineProcessingData> data) {
    enqueue(data);
}

optional<shared_ptr<PipelineProcessingData>> PipelineFiFo::fillIntoPipeline4SynchronousProcessing(shared_ptr<PipelineProcessingData> data) {
    LOGGER.warn("Synchronous processing is not supported by class PipelineFiFo. Payload will be dropped.");
    LOGGER.warn("Please use a FillerPipe derived class, that supports synchronous processing!");
    return nullopt;
}

} //namespace event_forge
