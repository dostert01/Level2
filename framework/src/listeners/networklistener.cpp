#include"networklistener.h"

using namespace std;

namespace event_forge {

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
} // namespace event_forge