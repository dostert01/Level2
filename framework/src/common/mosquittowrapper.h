#pragma once

#define MQTT_CONNECTOR_PARAM_NAME_HOSTNAME "hostName"
#define MQTT_CONNECTOR_PARAM_NAME_PORT "port"
#define MQTT_CONNECTOR_PARAM_NAME_CLIENT_ID "clientId"
#define MQTT_CONNECTOR_PARAM_NAME_TOPIC "topic"
#define PAYLOAD_NAME_MQTT_SEND_TEXT_DATA "MQTT_SEND_TEXT_DATA"

#include <memory>
#include <vector>
#include <shared_mutex>

#include "pipelinefifo.h"

namespace event_forge {
class MosquittoWrapper {
 public:
  static std::shared_mutex mqttConnectorInitMutex;
  static std::shared_ptr<MosquittoWrapper> getInstance(std::string hostName,
                                                  int port,
                                                  std::string clientId,
                                                  std::string topic);
  static std::shared_ptr<MosquittoWrapper> getInstance(std::string hostName,
                                                  int port,
                                                  std::string clientId,
                                                  std::vector<std::string> topics);
  MosquittoWrapper() = default;
  ~MosquittoWrapper();
  void init(std::string hostName, int port, std::string clientId, std::string topic);
  bool isInitComplete();
  void sendData(std::string payloadString);
  void startListening(std::shared_ptr<PipelineFiFo> fifo);
  bool isListening();
  std::string getHostName();
  std::optional<std::shared_ptr<PipelineFiFo>> getPipelineFifo();

 private:
  std::string hostName;
  int port;
  std::string clientId;
  std::vector<std::string> topics;
  bool initComplete;
  bool connected;
  bool listening;
  std::shared_ptr<PipelineFiFo> pipelineFifo;
  struct mosquitto *mosquittoHandle;
  void init(std::string hostName, int port, std::string clientId, std::vector<std::string>& topics);
  void initParams(std::string &hostName, int port, std::string &clientId, std::vector<std::string> topics);
  void setPipelineFifo(std::shared_ptr<PipelineFiFo> fifo);
  void initMosquittoLib();
  void initMosquittoConnection();
  void openMosquittoConnection();
  void createMosquittoHandle();
  void disconnectFromBroker();
  void startMosquittoLoop();
  void subscribeToAllTopics();
  void logMosquittoMessage(int mosquittoReturnCode, std::string successMessage, std::string errorMessage);
};
} // namespace event_forge
