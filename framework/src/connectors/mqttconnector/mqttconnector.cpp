#include <fstream>
#include <iostream>
#include <memory>
#include <mosquitto.h>
#include <pthread.h>
#include <error.h>
#include <string.h>

#include "pipelineapi.h"
#include "mqttconnector.h"
#include "logger.h"

/*
  for usage of mosquitto related stuff please refer to
  https://mosquitto.org/api/files/mosquitto-h.html
*/

namespace mqttconnector {
  std::string hostName;
  std::string port;
  std::string clientId;
  std::string topic;
  bool initComplete = true;
  bool isConnected = false;
  static pthread_rwlock_t mqttConnectorInitMutex = PTHREAD_RWLOCK_INITIALIZER;
  struct mosquitto* mosquittoHandle;
}

void initMosquittoLib() {
  if (mosquitto_lib_init() != MOSQ_ERR_SUCCESS) {
    LOGGER.error(
        "Failed to init mosquitto library. mqttconnecor will be disabled!");
    mqttconnector::initComplete = false;
  }
}

void initMosquittoConnection() {
  mqttconnector::mosquittoHandle = mosquitto_new(mqttconnector::clientId.c_str(), false, NULL);
  if(mqttconnector::mosquittoHandle == NULL) {
    LOGGER.error("Failed to get the mosquitto handle: " + std::string(strerror(errno)));
    mqttconnector::initComplete = false;
  }
  if(mqttconnector::initComplete) {
    if(mosquitto_connect(mqttconnector::mosquittoHandle,
      mqttconnector::hostName.c_str(),
      stoi(mqttconnector::port),
      60) != MOSQ_ERR_SUCCESS) {
        LOGGER.error("Failed to connect to the broker at hostName: '" +
          mqttconnector::hostName + "' : " + std::string(strerror(errno)));
        mqttconnector::initComplete = false;
    } else {
      LOGGER.info("Connected to MQTT broker at hostName: '" + mqttconnector::hostName + "'");
      mqttconnector::isConnected = true;
    }
  }
}

void initConnection() {
  pthread_rwlock_wrlock(&mqttconnector::mqttConnectorInitMutex);
  initMosquittoLib();
  pthread_rwlock_unlock(&mqttconnector::mqttConnectorInitMutex);
  initMosquittoConnection();
}

void readConnectionParams(PipelineStepInitData& initData) {
  std::optional<std::string> s = initData.getNamedArgument(MQTT_CONNECTOR_PARAM_NAME_HOSTNAME);
  if (s.has_value()) {
    mqttconnector::hostName = s.value();
  }
  s = initData.getNamedArgument(MQTT_CONNECTOR_PARAM_NAME_PORT);
  if (s.has_value()) {
    mqttconnector::port = s.value();
  }
  s = initData.getNamedArgument(MQTT_CONNECTOR_PARAM_NAME_CLIENT_ID);
  if (s.has_value()) {
    mqttconnector::clientId = s.value();
  }
  s = initData.getNamedArgument(MQTT_CONNECTOR_PARAM_NAME_TOPIC);
  if (s.has_value()) {
    mqttconnector::topic = s.value();
  }
  if(mqttconnector::hostName.empty() || mqttconnector::port.empty() || mqttconnector::clientId.empty() || mqttconnector::topic.empty()) {
    LOGGER.error("Failed loading init parameters for connection to MQTT broker:");
  } else {
    LOGGER.info("Loading of init parameters for connection to MQTT broker done:");
  }
  LOGGER.info("hostName: " + mqttconnector::hostName);
  LOGGER.info("port: " + mqttconnector::port);
  LOGGER.info("clientId: " + mqttconnector::clientId);
  LOGGER.info("topic: " + mqttconnector::topic);
}

int pipeline_step_module_init(PipelineStepInitData& initData) {
  readConnectionParams(initData);
  initConnection();
  return 0;
}

void sendData(PipelineProcessingData& processData) {
  optional<shared_ptr<ProcessingPayload>> payload = processData.getPayload(PAYLOAD_NAME_MQTT_SEND_TEXT_DATA);
  if(payload.has_value()) {
    LOGGER.info("sending text payload to MQTT broker.");
    string payloadString = payload.value()->payloadAsString();
    int returnCode = mosquitto_publish(
      mqttconnector::mosquittoHandle,
      NULL,
      mqttconnector::topic.c_str(),
      strlen(payloadString.c_str()) + 1,
      payloadString.c_str(),
      0,
      false);
    if(returnCode == MOSQ_ERR_SUCCESS) {
      LOGGER.info("Message sent successful");
    } else {
      LOGGER.error("Failed sending message to broker. Errorcode: " + to_string(returnCode));
    }
  } else {
    LOGGER.warn("No suitable payload with name '" +
      string(PAYLOAD_NAME_MQTT_SEND_TEXT_DATA) + "' contained in processing data. Nothing can be sent!");
  }
}

int pipeline_step_module_process(PipelineProcessingData& processData) {
  if(mqttconnector::isConnected) {
    sendData(processData);
  } else {
    LOGGER.warn("Client is not connected to the MQTT broker. No data can be sent.");
  }
  return 0;
}

int pipeline_step_module_finish() {
  if(mqttconnector::isConnected) {
    LOGGER.info("Disconnecting from MQTT broker at "+ mqttconnector::hostName);
    mosquitto_disconnect(mqttconnector::mosquittoHandle);
    mqttconnector::isConnected = false;
  }
  mosquitto_destroy(mqttconnector::mosquittoHandle);
  mosquitto_lib_cleanup();
  return 0;
}

