#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include <thread>
#include <vector>

#include "generic_server.h"
#include "http11.h"

using namespace nlohmann::json_abi_v3_11_3;

namespace event_forge {

class HTTPListener : public GenericServer {
 public:
  HTTPListener() = default;
  HTTPListener(json jsonObject);
  ~HTTPListener();
  static std::shared_ptr<HTTPListener> getInstance();
  std::optional<std::shared_ptr<PipelineProcessingData>> getLastProcessingData();
 private:
  std::shared_ptr<PipelineProcessingData> processingData;
  void handleClientConnection(int clientSocket, std::string clientHost) override;
  void processData();
  void sendResponse(Http11 &http);
  bool errorsOccurredDuringProcessing();
  void handleProcessingErrors(event_forge::HttpResponse &response);
  void setProcessingDataPayloadAsResponsePayload(HttpResponse &response);
  void handleNotProcessedError(HttpResponse &response);
  bool processingDataHasNotBeenProcessed();
  void handleRequestParsingError(Http11 &http, HttpResponse &response);
  bool errorDuringRequestParsing(Http11 &http);
  void preparePayloadString(std::shared_ptr<HttpRequest> request, std::string &payload);
  void setPayloadParameters(std::shared_ptr<HttpRequest> request, std::string &clientHost);
  void collectHeaderPayloadParameters(std::map<std::string, std::string> &payloadParams, std::shared_ptr<HttpRequest> &request);
  void collectUrlPayloadParameters(std::shared_ptr<HttpRequest> &request, std::map<std::string, std::string> &payloadParams);
  void collectDefaultPayloadParameters(std::map<std::string, std::string> &payloadParams, std::string &clientHost);
  void prepareProcessingData(std::shared_ptr<HttpRequest> request, std::string &payload, std::string &clientHost);
};

}  // namespace 