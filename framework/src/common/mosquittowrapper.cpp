/*
  for usage of mosquitto related stuff please refer to
  https://mosquitto.org/api/files/mosquitto-h.html
*/

#include <error.h>
#include <mosquitto.h>
#include <string.h>

#include "logger.h"
#include "mosquittowrapper.h"
#include "payloadnames.h"

namespace level2 {

std::shared_mutex MosquittoWrapper::mqttConnectorInitMutex;

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
  MosquittoWrapper *mqtt = (MosquittoWrapper*)obj;

  if(mqtt != NULL && mqtt->getPipelineFifo() != std::nullopt) {

    LOGGER.debug("New MQTT message received for topic '" + std::string(msg->topic) + "'. "
      "Message payload: " + std::string((char*)msg->payload));

    std::map<std::string, std::string> matchingPatterns;
    matchingPatterns[PAYLOAD_MATCHING_PATTERN_DATA_ORIGIN] =
      std::string(msg->topic);
    matchingPatterns[PAYLOAD_MATCHING_PATTERN_RECEIVED_BY_LISTENER] =
      PAYLOAD_MATCHING_PATTERN_VALUE_MQTT_LISTENER;
    matchingPatterns[PAYLOAD_MATCHING_PATTERN_RECEIVED_VIA_PROTOCOL] =
      PAYLOAD_MATCHING_PATTERN_VALUE_PROTOCOL_MQTT;
    matchingPatterns[PAYLOAD_MATCHING_PATTERN_RECEIVED_FROM_HOST] =
      mqtt->getHostName();

    mqtt->getPipelineFifo().value()->enqueue(
      PAYLOAD_NAME_MQTT_RECEIVED_DATA,
      PAYLOAD_MIMETYPE_TEXT_PLAIN,
      std::string((char*)msg->payload),
      matchingPatterns);

  } else {
    LOGGER.warn("New MQTT message received for topic '" + std::string(msg->topic) + "'. "
      "However, the message can not be forwarded because no valid instance of "
      "MosquittoWrapper is available. The message will be dropped!");

  }
}

std::shared_ptr<MosquittoWrapper> MosquittoWrapper::getInstance(std::string hostName,
                                                                int port,
                                                                std::string clientId,
                                                                std::string topic) {
  std::shared_ptr<MosquittoWrapper> instance = std::make_shared<MosquittoWrapper>();
  std::vector<std::string> topics;
  topics.push_back(topic);
  instance->init(hostName, port, clientId, topics);
  return instance;
}

std::shared_ptr<MosquittoWrapper> MosquittoWrapper::getInstance(std::string hostName,
                                                                int port,
                                                                std::string clientId,
                                                                std::vector<std::string> topics) {
  std::shared_ptr<MosquittoWrapper> instance = std::make_shared<MosquittoWrapper>();
  instance->init(hostName, port, clientId, topics);
  return instance;
}

MosquittoWrapper::~MosquittoWrapper() {
  disconnectFromBroker();
  mosquitto_destroy(mosquittoHandle);
  mosquitto_lib_cleanup();
}

void MosquittoWrapper::init(std::string hostName, int port, std::string clientId, std::string topic) {
  std::vector<std::string> justOneTopic;
  justOneTopic.push_back(topic);
  init(hostName, port, clientId, justOneTopic);
}

void MosquittoWrapper::init(std::string hostName, int port, std::string clientId, std::vector<std::string>& topics) {
  initComplete = true;
  connected = false;
  listening = false;
  initParams(hostName, port, clientId, topics);
  initMosquittoLib();
  initMosquittoConnection();
}

void MosquittoWrapper::initMosquittoLib() {
  std::unique_lock<std::shared_mutex> lock(mqttConnectorInitMutex);
  if (initComplete && (mosquitto_lib_init() != MOSQ_ERR_SUCCESS)) {
    LOGGER.error(
        "Failed to init mosquitto library. mqtt will be disabled!");
    initComplete = false;
  }
}

void MosquittoWrapper::initMosquittoConnection() {
  createMosquittoHandle();
  openMosquittoConnection();
}

void MosquittoWrapper::createMosquittoHandle() {
  mosquittoHandle = mosquitto_new(clientId.c_str(), false, this);
  if (mosquittoHandle == NULL) {
    LOGGER.error("Failed to get the mosquitto handle: " +
                 std::string(strerror(errno)));
    initComplete = false;
  }
}

void MosquittoWrapper::openMosquittoConnection() {
  if (initComplete) {
    if (mosquitto_connect(mosquittoHandle, hostName.c_str(), port, 60) !=
        MOSQ_ERR_SUCCESS) {
      LOGGER.error("Failed to connect to the broker at hostName: '" + hostName +
                   "' : " + std::string(strerror(errno)));
      initComplete = false;
    } else {
      LOGGER.info("Connected to MQTT broker at hostName: '" + hostName + "'");
      connected = true;
    }
  }
}

void MosquittoWrapper::setPipelineFifo(std::shared_ptr<PipelineFiFo> fifo) {
  pipelineFifo = fifo;
}

std::string MosquittoWrapper::getHostName() {
  return hostName;
}

std::optional<std::shared_ptr<PipelineFiFo>> MosquittoWrapper::getPipelineFifo() {
  if(pipelineFifo == nullptr) {
    LOGGER.warn("this instance of MosquittoWrapper does not have an instance of PipelineFiFo attached. No data can be forwarded to a processing pipeline!");
    return std::nullopt;
  } else {
    return pipelineFifo;
  }
}

void MosquittoWrapper::startListening(std::shared_ptr<PipelineFiFo> fifo) {
  setPipelineFifo(fifo);
  if(!listening && connected) {
    mosquitto_message_callback_set(mosquittoHandle, on_message);
    subscribeToAllTopics();
    startMosquittoLoop();
  }
}

void MosquittoWrapper::startMosquittoLoop() {
  int success = mosquitto_loop_start(mosquittoHandle);
  logMosquittoMessage(success, "mosquitto loop started",
                      "failed starting mosquitto loop");
  listening = (success == MOSQ_ERR_SUCCESS);
}

void MosquittoWrapper::subscribeToAllTopics() {
  for (std::string topic : topics) {
    int success = mosquitto_subscribe(mosquittoHandle, NULL, topic.c_str(), 0);
    logMosquittoMessage(success, "subscribed to topic '" + topic + "'",
                        "failed to subscribed to topic '" + topic + "'");
  }
}

void MosquittoWrapper::logMosquittoMessage(int mosquittoReturnCode, std::string successMessage, std::string errorMessage) {
    if(mosquittoReturnCode == MOSQ_ERR_SUCCESS) {
      LOGGER.info(successMessage);
    } else {
      LOGGER.error(errorMessage + " - errorcode: " + std::string(mosquitto_strerror(mosquittoReturnCode)));
    }
}

void MosquittoWrapper::disconnectFromBroker() {
  if(connected) {
    LOGGER.info("Disconnecting from MQTT broker at " + hostName);
    mosquitto_disconnect(mosquittoHandle);
    connected = false;
  }
}

void MosquittoWrapper::sendData(std::string payloadString) {
  int returnCode = mosquitto_publish(
      mosquittoHandle, NULL, topics[0].c_str(),
      strlen(payloadString.c_str()) + 1, payloadString.c_str(), 0, false);
  if (returnCode == MOSQ_ERR_SUCCESS) {
    LOGGER.info("Message sent successful");
  } else {
    LOGGER.error("Failed sending message to broker. Errorcode: " +
        std::to_string(returnCode));
  }
}

bool MosquittoWrapper::isListening() {
  return listening;
}

void MosquittoWrapper::initParams(std::string &hostName, int port,
                                  std::string &clientId, std::vector<std::string> topics) {
  this->hostName = hostName;
  this->port = port;
  this->clientId = clientId;
  this->topics = topics;
}

bool MosquittoWrapper::isInitComplete() {
  return initComplete && connected;
}
} // namespace level2