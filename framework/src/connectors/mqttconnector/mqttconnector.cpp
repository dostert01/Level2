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

namespace event_forge {

std::shared_ptr<MosquittoWrapper> mqtt;

string getNamedArgument(PipelineStepInitData& initData, string paramName) {
  string value = initData.getNamedArgument(paramName).value_or("");
  LOGGER.info(paramName + ": " + value);
  return value;
}

void initMQTTConnection(PipelineStepInitData& initData) {
  string hostName = getNamedArgument(initData, MQTT_CONNECTOR_PARAM_NAME_HOSTNAME);
  string port = getNamedArgument(initData, MQTT_CONNECTOR_PARAM_NAME_PORT);
  string clientId = getNamedArgument(initData, MQTT_CONNECTOR_PARAM_NAME_CLIENT_ID);
  string topic = getNamedArgument(initData, MQTT_CONNECTOR_PARAM_NAME_TOPIC);

  if(hostName.empty() || port.empty() || clientId.empty() || topic.empty()) {
    LOGGER.error("Failed loading init parameters for connection to MQTT broker:");
  } else {
    LOGGER.info("Loading of init parameters for connection to MQTT broker done:");
  }
  mqtt = MosquittoWrapper::getInstance(hostName, stoi(port), clientId, topic);
}

void sendData(PipelineProcessingData& processData) {
  auto payload = processData.getPayload(PAYLOAD_NAME_MQTT_SEND_TEXT_DATA);
  if(payload.has_value()) {
    LOGGER.info("sending text payload to MQTT broker.");
    mqtt->sendData(payload.value()->payloadAsString());
  } else {
    LOGGER.warn("No suitable payload with name '" +
      string(PAYLOAD_NAME_MQTT_SEND_TEXT_DATA) + "' contained in processing data. Nothing can be sent!");
  }
}

} // namespace event_forge

int pipeline_step_module_init(event_forge::PipelineStepInitData& initData) {
  event_forge::initMQTTConnection(initData);
  return 0;
}

int pipeline_step_module_process(event_forge::PipelineProcessingData& processData) {
  if(event_forge::mqtt->isInitComplete()) {
    event_forge::sendData(processData);
  } else {
    event_forge::LOGGER.warn("Client is not connected to the MQTT broker. No data can be sent.");
  }
  return 0;
}

int pipeline_step_module_finish() {
  return 0;
}

