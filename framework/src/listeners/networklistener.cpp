#include"networklistener.h"

void NetworkListener::init(shared_ptr<FillerPipe> fifo) {
    pipelineFifo = dynamic_pointer_cast<PipelineFiFo>(fifo);
}

optional<shared_ptr<PipelineProcessingData>> NetworkListener::getLastMessage() {
    if(pipelineFifo != nullptr) {
        return pipelineFifo->dequeue();
    } else {
        return nullopt;
    }
}