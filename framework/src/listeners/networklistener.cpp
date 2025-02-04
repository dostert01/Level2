#include"networklistener.h"

using namespace std;

namespace level2 {

NetworkListener::NetworkListener() {
    processingMode = ListenerProcessingMode::not_set;
}

void NetworkListener::init(shared_ptr<PipelineFiFo> fifo) {
    pipelineFifo = fifo;
    processingMode = ListenerProcessingMode::asynchronous;
}

void NetworkListener::init(std::shared_ptr<PipeLineProcessor> processor){
    pipeLineProcessor = processor;
    processingMode = ListenerProcessingMode::synchronous;
}

optional<shared_ptr<PipelineProcessingData>> NetworkListener::getLastMessage() {
    if(pipelineFifo != nullptr && processingMode == ListenerProcessingMode::asynchronous) {
        return pipelineFifo->dequeue();
    } else {
        return nullopt;
    }
}

bool NetworkListener::isListening() {
    return listening;
}

bool NetworkListener::isIniComplete() {
    return initComplete;
}

} // namespace level2