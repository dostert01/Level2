#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <memory>

#include "applicationcontext.h"
#include "httplistener.h"
#include "payloadnames.h"

namespace event_forge {

HTTPListener::HTTPListener(json jsonObject) : GenericServer(jsonObject) {
};

HTTPListener::~HTTPListener() {
}

std::shared_ptr<HTTPListener> HTTPListener::getInstance() {
  return std::make_shared<HTTPListener>();
}

void HTTPListener::handleClientConnection(int clientSocket, std::string clientHost) {
  Http11 http;
  std::string payload = "";
  auto request = http.readRequest(clientSocket, clientHost);

  if(request.has_value()) {
    preparePayloadString(request, payload);
    prepareProcessingData(payload, clientHost);
    processData();
  } else {
    LOGGER.error("failed to load request header during processing of client request.");
  }
}

void HTTPListener::processData() {
  if (processingMode == ListenerProcessingMode::synchronous) {
    LOGGER.trace("starting synchronous processing");
    pipeLineProcessor->execute(processingData);
  } else if (processingMode == ListenerProcessingMode::asynchronous) {
    LOGGER.trace("starting asynchronous processing");
    pipelineFifo->enqueue(processingData);
  }
}

void HTTPListener::prepareProcessingData(std::string &payload, std::string &clientHost) {
    processingData = PipelineProcessingData::getInstance();
    setPayloadParameters(clientHost);
    processingData->addPayloadData(
      PAYLOAD_NAME_HTTP_RECEIVED_DATA,
      PAYLOAD_MIMETYPE_APPLICATION_OCTET_STREAM,
      payload);
}

void HTTPListener::preparePayloadString(std::optional<std::shared_ptr<HttpRequest>> &request, std::string &payload) {
  if (request.value()->hasPayload()) {
    payload = std::string(request.value()->getContentPointer().value());
  }
}

void HTTPListener::setPayloadParameters(std::string &clientHost) {
  std::map<std::string, std::string> params;
  params[PAYLOAD_MATCHING_PATTERN_DATA_ORIGIN] = clientHost;
  params[PAYLOAD_MATCHING_PATTERN_RECEIVED_BY_LISTENER] =
      PAYLOAD_MATCHING_PATTERN_VALUE_HTTP_LISTENER;
  params[PAYLOAD_MATCHING_PATTERN_RECEIVED_VIA_PROTOCOL] =
      PAYLOAD_MATCHING_PATTERN_VALUE_PROTOCOL_HTTP;
  processingData->addMatchingPatterns(params);
}

std::shared_ptr<PipelineProcessingData> HTTPListener::getLastProcessingData() {
  return processingData;
}

} // namespace event_forge