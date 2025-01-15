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

using namespace event_forge;

shared_mutex MosquittoWrapper::mqttConnectorInitMutex;

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
  MosquittoWrapper *mqtt = (MosquittoWrapper*)obj;

  if(mqtt != NULL && mqtt->getPipelineFifo() != nullopt) {

    LOGGER.debug("New MQTT message received for topic '" + string(msg->topic) + "'. "
      "Message payload: " + string((char*)msg->payload));

    map<string, string> matchingPatterns;
    matchingPatterns[PAYLOAD_MATCHIN_PATTERN_DATA_ORIGIN] =
      string(msg->topic);
    matchingPatterns[PAYLOAD_MATCHIN_PATTERN_RECEIVED_BY_LISTENER] =
      PAYLOAD_MATCHIN_PATTERN_VALUE_MQTT_LISTENER;
    matchingPatterns[PAYLOAD_MATCHIN_PATTERN_RECEIVED_VIA_PROTOCOL] =
      PAYLOAD_MATCHIN_PATTERN_VALUE_PROTOCOL_MQTT;
    matchingPatterns[PAYLOAD_MATCHIN_PATTERN_RECEIVED_FROM_HOST] =
      mqtt->getHostName();

    mqtt->getPipelineFifo().value()->enqueue(
      PAYLOAD_NAME_MQTT_RECEIVED_DATA,
      PAYLOAD_MIMETYPE_TEXT_PLAIN,
      string((char*)msg->payload),
      matchingPatterns);

  } else {

    LOGGER.warn("New MQTT message received for topic '" + string(msg->topic) + "'. "
      "However, the message can not be forwarded because no valid instance of "
      "MosquittoWrapper is available. The message will be dropped!");

  }
}

shared_ptr<MosquittoWrapper> MosquittoWrapper::getInstance(string hostName,
                                                                int port,
                                                                string clientId,
                                                                string topic) {
  shared_ptr<MosquittoWrapper> instance = make_shared<MosquittoWrapper>();
  vector<string> topics;
  topics.push_back(topic);
  instance->init(hostName, port, clientId, topics);
  return instance;
}

shared_ptr<MosquittoWrapper> MosquittoWrapper::getInstance(string hostName,
                                                                int port,
                                                                string clientId,
                                                                vector<string> topics) {
  shared_ptr<MosquittoWrapper> instance = make_shared<MosquittoWrapper>();
  instance->init(hostName, port, clientId, topics);
  return instance;
}

MosquittoWrapper::~MosquittoWrapper() {
  disconnectFromBroker();
  mosquitto_destroy(mosquittoHandle);
  mosquitto_lib_cleanup();
}

void MosquittoWrapper::init(string hostName, int port, string clientId, string topic) {
  vector<string> justOneTopic;
  justOneTopic.push_back(topic);
  init(hostName, port, clientId, justOneTopic);
}

void MosquittoWrapper::init(string hostName, int port, string clientId, vector<string>& topics) {
  initComplete = true;
  connected = false;
  listening = false;
  initParams(hostName, port, clientId, topics);
  initMosquittoLib();
  initMosquittoConnection();
}

void MosquittoWrapper::initMosquittoLib() {
  unique_lock<shared_mutex> lock(mqttConnectorInitMutex);
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
                 string(strerror(errno)));
    initComplete = false;
  }
}

void MosquittoWrapper::openMosquittoConnection() {
  if (initComplete) {
    if (mosquitto_connect(mosquittoHandle, hostName.c_str(), port, 60) !=
        MOSQ_ERR_SUCCESS) {
      LOGGER.error("Failed to connect to the broker at hostName: '" + hostName +
                   "' : " + string(strerror(errno)));
      initComplete = false;
    } else {
      LOGGER.info("Connected to MQTT broker at hostName: '" + hostName + "'");
      connected = true;
    }
  }
}

void MosquittoWrapper::setPipelineFifo(shared_ptr<PipelineFiFo> fifo) {
  pipelineFifo = fifo;
}

string MosquittoWrapper::getHostName() {
  return hostName;
}

optional<shared_ptr<PipelineFiFo>> MosquittoWrapper::getPipelineFifo() {
  if(pipelineFifo == nullptr) {
    LOGGER.warn("this instance of MosquittoWrapper does not have an instance of PipelineFiFo attached. No data can be forwarded to a processing pipeline!");
    return nullopt;
  } else {
    return pipelineFifo;
  }
}

void MosquittoWrapper::startListening(shared_ptr<PipelineFiFo> fifo) {
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
  for (string topic : topics) {
    int success = mosquitto_subscribe(mosquittoHandle, NULL, topic.c_str(), 0);
    logMosquittoMessage(success, "subscribed to topic '" + topic + "'",
                        "failed to subscribed to topic '" + topic + "'");
  }
}

void MosquittoWrapper::logMosquittoMessage(int mosquittoReturnCode, string successMessage, string errorMessage) {
    if(mosquittoReturnCode == MOSQ_ERR_SUCCESS) {
      LOGGER.info(successMessage);
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

bool MosquittoWrapper::isInitComplete() {
  return initComplete && connected;
}
