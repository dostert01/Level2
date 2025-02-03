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
#include "httpdefs.h"

namespace event_forge {

HTTPListener::HTTPListener(json jsonObject) : GenericServer(jsonObject) {
};

HTTPListener::~HTTPListener() {
}

std::shared_ptr<HTTPListener> HTTPListener::getInstance() {
  return std::make_shared<HTTPListener>();
}

void HTTPListener::handleClientConnection(int clientSocket, std::string clientHost) {
  Http11 http(clientSocket);
  std::string payload = "";
  auto request = http.readRequest(clientHost);

  if(request.has_value()) {
    preparePayloadString(request.value(), payload);
    prepareProcessingData(request.value(), payload, clientHost);
    processData();
  } else {
    LOGGER.error("failed to load request header during processing of client request.");
  }
  sendResponse(http);
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

void HTTPListener::prepareProcessingData(std::shared_ptr<HttpRequest> request, std::string &payload, std::string &clientHost) {
    processingData = PipelineProcessingData::getInstance();
    setPayloadParameters(request, clientHost);
    processingData->addPayloadData(
      PAYLOAD_NAME_HTTP_RECEIVED_DATA,
      PAYLOAD_MIMETYPE_APPLICATION_OCTET_STREAM,
      payload);
}

void HTTPListener::preparePayloadString(std::shared_ptr<HttpRequest> request, std::string &payload) {
  if (request->hasPayload()) {
    payload = std::string(request->getContentPointer().value());
  }
}

void HTTPListener::sendResponse(Http11 &http) {
  HttpResponse response;
  if (processingMode == ListenerProcessingMode::synchronous) {
    if (errorDuringRequestParsing(http)) {
      handleRequestParsingError(http, response);
    } else if (processingDataHasNotBeenProcessed()) {
      handleNotProcessedError(response);
    } else if (errorsOccurredDuringProcessing()) {
      handleProcessingErrors(response);
    } else {
      setProcessingDataPayloadAsResponsePayload(response);
    }
  } else if (processingMode == ListenerProcessingMode::asynchronous) {
    
  }
  http.sendResponse(response);
}

bool HTTPListener::errorsOccurredDuringProcessing() {
  return processingData->hasError();
}

void HTTPListener::setProcessingDataPayloadAsResponsePayload(HttpResponse &response) {
  auto payload = processingData->getLastPayload();
  if (payload.has_value()) {
    response.setPayload(payload.value()->payloadAsString());
  }
}

void HTTPListener::handleProcessingErrors(event_forge::HttpResponse &response) {
  response.setStatusCode(HTTP_STATUS_CODE_500);
  std::string errorPayload = HTTP_STATUS_CODE_500;
  for (auto currentError : processingData->getAllErrors()) {
    errorPayload.append("\n");
    errorPayload.append(currentError->getErrorCode());
    errorPayload.append(" : ");
    errorPayload.append(currentError->getErrorMessage());
  }
  response.setPayload(errorPayload);
}

void HTTPListener::handleNotProcessedError(HttpResponse &response) {
  response.setStatusCode(HTTP_STATUS_CODE_404);
  response.setPayload(std::string(HTTP_STATUS_CODE_404) + " \nNo processing pipeline matches the request data");
}

bool HTTPListener::processingDataHasNotBeenProcessed() {
  return processingData->getProcessingCounter() == 0;
}

void HTTPListener::handleRequestParsingError(Http11 &http, HttpResponse &response) {
  HttpException ex = http.getLastError().value_or(HttpException(HTTP_STATUS_CODE_200, ""));
  response.setStatusCode(ex.getHttpReturnCode());
  response.setPayload(ex.getHttpReturnCode() + " \n" + ex.what());
}

bool HTTPListener::errorDuringRequestParsing(Http11 &http) {
  return http.hasErrors();
}

void HTTPListener::setPayloadParameters(std::shared_ptr<HttpRequest> request, std::string &clientHost) {
  std::map<std::string, std::string> payloadParams;
  collectDefaultPayloadParameters(payloadParams, clientHost);
  collectUrlPayloadParameters(request, payloadParams);
  collectHeaderPayloadParameters(payloadParams, request);
  processingData->addMatchingPatterns(payloadParams);
}

void HTTPListener::collectHeaderPayloadParameters(std::map<std::string, std::string> &payloadParams, std::shared_ptr<HttpRequest> &request) {
  payloadParams[PAYLOAD_MATCHING_PATTERN_HTTP_PATH] = request->getPath();
  payloadParams[PAYLOAD_MATCHING_PATTERN_HTTP_METHOD] = request->getMethod();
  auto headerFields = request->getAllHeaderFields();
  for (auto headerField : *headerFields) {
    payloadParams[PAYLOAD_MATCHING_PATTERN_HTTP_HEADER_FIELD_PREFIX + headerField.first] = headerField.second;
  }
}

void HTTPListener::collectUrlPayloadParameters(std::shared_ptr<HttpRequest> &request, std::map<std::string, std::string> &payloadParams) {
  auto urlParams = request->getAllUrlParams();
  for (auto urlParam = urlParams->begin(); urlParam != urlParams->end(); ++urlParam) {
    payloadParams[PAYLOAD_MATCHING_PATTERN_HTTP_URL_PARAMS_PREFIX + urlParam->first] = urlParam->second;
  }
}

void HTTPListener::collectDefaultPayloadParameters(std::map<std::string, std::string> &payloadParams, std::string &clientHost) {
  payloadParams[PAYLOAD_MATCHING_PATTERN_DATA_ORIGIN] = clientHost;
  payloadParams[PAYLOAD_MATCHING_PATTERN_RECEIVED_BY_LISTENER] = PAYLOAD_MATCHING_PATTERN_VALUE_HTTP_LISTENER;
  payloadParams[PAYLOAD_MATCHING_PATTERN_RECEIVED_VIA_PROTOCOL] = PAYLOAD_MATCHING_PATTERN_VALUE_PROTOCOL_HTTP;
}

std::optional<std::shared_ptr<PipelineProcessingData>> HTTPListener::getLastProcessingData() {
  std::optional<std::shared_ptr<PipelineProcessingData>> returnValue = std::nullopt;
  if(processingData) {
    returnValue = processingData;
  }
  return returnValue;
}

} // namespace event_forge