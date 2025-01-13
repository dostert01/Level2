/*
  for usage of mosquitto related stuff please refer to
  https://mosquitto.org/api/files/mosquitto-h.html
*/

#include <error.h>
#include <mosquitto.h>
#include <string.h>

#include "logger.h"
#include "mosquittowrapper.h"

using namespace event_forge;

std::shared_mutex MosquittoWrapper::mqttConnectorInitMutex;

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
}

std::shared_ptr<MosquittoWrapper> MosquittoWrapper::getInstance(string hostName,
                                                                int port,
                                                                string clientId,
                                                                string topic) {
  std::shared_ptr<MosquittoWrapper> instance = std::make_shared<MosquittoWrapper>();
  vector<string> topics;
  topics.push_back(topic);
  instance->init(hostName, port, clientId, topics);
  return instance;
}

std::shared_ptr<MosquittoWrapper> MosquittoWrapper::getInstance(string hostName,
                                                                int port,
                                                                string clientId,
                                                                vector<string> topics) {
  std::shared_ptr<MosquittoWrapper> instance = std::make_shared<MosquittoWrapper>();
  instance->init(hostName, port, clientId, topics);
  return instance;
}

MosquittoWrapper::~MosquittoWrapper() {
  disconnectFromBroker();
  mosquitto_destroy(mosquittoHandle);
  mosquitto_lib_cleanup();
}

void MosquittoWrapper::startListening(shared_ptr<FillerPipe> fillerPipe) {
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
  for (string topic : topics) {
    int success = mosquitto_subscribe(mosquittoHandle, NULL, topic.c_str(), 0);
    logMosquittoMessage(success, "subscribed to topic '" + topic + "'",
                        "failed to subscribed to topic '" + topic + "'");
  }
}

void MosquittoWrapper::logMosquittoMessage(int mosquittoReturnCode, string sucessMessage, string errorMessage) {
    if(mosquittoReturnCode == MOSQ_ERR_SUCCESS) {
      LOGGER.info(sucessMessage);
    } else {
      LOGGER.error(errorMessage + " - errorcode: " + string(mosquitto_strerror(mosquittoReturnCode)));
    }
}

void MosquittoWrapper::disconnectFromBroker() {
  if(connected) {
    LOGGER.info("Disconnecting from MQTT broker at " + hostName);
    mosquitto_disconnect(mosquittoHandle);
    connected = false;
  }
}

void MosquittoWrapper::init(string hostName, int port, string clientId, vector<string>& topics) {
  initComplete = true;
  connected = false;
  listening = false;
  initParams(hostName, port, clientId, topics);
  initMosquittoLib();
  initMosquittoConnection();
}

void MosquittoWrapper::sendData(string payloadString) {
  int returnCode = mosquitto_publish(
      mosquittoHandle, NULL, topics[0].c_str(),
      strlen(payloadString.c_str()) + 1, payloadString.c_str(), 0, false);
  if (returnCode == MOSQ_ERR_SUCCESS) {
    LOGGER.info("Message sent successful");
  } else {
    LOGGER.error("Failed sending message to broker. Errorcode: " +
        to_string(returnCode));
  }
}

void MosquittoWrapper::initMosquittoConnection() {
  createMosquittoHandle();
  openMosquittoConnection();
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

void MosquittoWrapper::createMosquittoHandle() {
  mosquittoHandle = mosquitto_new(clientId.c_str(), false, NULL);
  if (mosquittoHandle == NULL) {
    LOGGER.error("Failed to get the mosquitto handle: " +
                 std::string(strerror(errno)));
    initComplete = false;
  }
}

bool MosquittoWrapper::isListening() {
  return listening;
}

void MosquittoWrapper::initParams(string &hostName, int port,
                                  string &clientId, vector<string> topics) {
  this->hostName = hostName;
  this->port = port;
  this->clientId = clientId;
  this->topics = topics;
}

void MosquittoWrapper::initMosquittoLib() {
  std::unique_lock<std::shared_mutex> lock(mqttConnectorInitMutex);
  if (initComplete && (mosquitto_lib_init() != MOSQ_ERR_SUCCESS)) {
    LOGGER.error(
        "Failed to init mosquitto library. mqtt will be disabled!");
    initComplete = false;
  }
}

bool MosquittoWrapper::isInitComplete() {
  return initComplete && connected;
}
