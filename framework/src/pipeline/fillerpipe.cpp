
#include "fillerpipe.h"

using namespace event_forge;

void FillerPipe::addData(string payloadName, string mimetype, string payload) {
    shared_ptr<PipelineProcessingData> data = make_shared<PipelineProcessingData>();
    data->addPayloadData(payloadName, mimetype, payload);
    add2Fifo(data);
}
