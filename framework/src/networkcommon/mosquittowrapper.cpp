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

std::shared_ptr<MosquittoWrapper> MosquittoWrapper::getInstance(string hostName,
                                                                int port,
                                                                string clientId,
                                                                string topic) {
  std::shared_ptr<MosquittoWrapper> instance = std::make_shared<MosquittoWrapper>();
  instance->init(hostName, port, clientId, topic);
  return instance;
}

MosquittoWrapper::~MosquittoWrapper() {
  disconnectFromBroker();
  mosquitto_destroy(mosquittoHandle);
  mosquitto_lib_cleanup();
}

void MosquittoWrapper::disconnectFromBroker() {
  if(isConnected) {
    LOGGER.info("Disconnecting from MQTT broker at " + hostName);
    mosquitto_disconnect(mosquittoHandle);
    isConnected = false;
  }
}

void MosquittoWrapper::init(string hostName, int port, string clientId, string topic) {
  initComplete = true;
  isConnected = false;
  initParams(hostName, port, clientId, topic);
  initMosquittoLib();
  initMosquittoConnection();
}

void MosquittoWrapper::sendData(string payloadString) {
  int returnCode = mosquitto_publish(
      mosquittoHandle, NULL, topic.c_str(),
      strlen(payloadString.c_str()) + 1, payloadString.c_str(), 0, false);
  if (returnCode == MOSQ_ERR_SUCCESS) {
    LOGGER.info("Message sent successful");
  } else {
    LOGGER.error("Failed sending message to broker. Errorcode: " +
        to_string(returnCode));
  }
}

void MosquittoWrapper::initMosquittoConnection() {
  mosquittoHandle = mosquitto_new(clientId.c_str(), false, NULL);
  if (mosquittoHandle == NULL) {
    LOGGER.error("Failed to get the mosquitto handle: " +
                 std::string(strerror(errno)));
    initComplete = false;
  }
  if (initComplete) {
    if (mosquitto_connect(mosquittoHandle, hostName.c_str(), port, 60) !=
        MOSQ_ERR_SUCCESS) {
      LOGGER.error("Failed to connect to the broker at hostName: '" + hostName +
                   "' : " + std::string(strerror(errno)));
      initComplete = false;
    } else {
      LOGGER.info("Connected to MQTT broker at hostName: '" + hostName + "'");
      isConnected = true;
    }
  }
}

void MosquittoWrapper::initParams(std::string &hostName, int port,
                                  std::string &clientId, std::string &topic) {
  this->hostName = hostName;
  this->port = port;
  this->clientId = clientId;
  this->topic = topic;
}

void MosquittoWrapper::initMosquittoLib() {
  std::unique_lock<std::shared_mutex> lock(mqttConnectorInitMutex);
  if (initComplete && (mosquitto_lib_init() != MOSQ_ERR_SUCCESS)) {
    LOGGER.error(
        "Failed to init mosquitto library. mqttconnecor will be disabled!");
    initComplete = false;
  }
}

bool MosquittoWrapper::isInitComplete() {
  return initComplete && isConnected;
}
