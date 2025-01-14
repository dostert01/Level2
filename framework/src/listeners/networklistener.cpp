#include"networklistener.h"

void NetworkListener::init(shared_ptr<PipelineFiFo> fifo) {
    pipelineFifo = fifo;
}

optional<shared_ptr<PipelineProcessingData>> NetworkListener::getLastMessage() {
    if(pipelineFifo != nullptr) {
        return pipelineFifo->dequeue();
    } else {
        return nullopt;
    }
}