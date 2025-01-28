#include "httplistener.h"
#include "applicationcontext.h"

namespace event_forge {

HTTPListener::HTTPListener(json jsonObject) {
  port = jsonObject["port"];
};

std::shared_ptr<HTTPListener> HTTPListener::getInstance() {
  return std::make_shared<HTTPListener>();
}

void HTTPListener::init(std::shared_ptr<PipelineFiFo> fifo) {
    NetworkListener::init(fifo);
    initComplete = true;
}

void HTTPListener::startListening() {
}

int HTTPListener::getPort() {
    return port;
}

} // namespace event_forge