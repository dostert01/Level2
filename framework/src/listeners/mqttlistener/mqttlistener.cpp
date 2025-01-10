#include "mqttlistener.h"

using namespace std;

MQTTListener::MQTTListener(json jsonObject) {
  hostName = jsonObject["hostName"];
  port = jsonObject["port"];
  clientId = jsonObject["clientId"];
  topic = jsonObject["topic"];
};

shared_ptr<MQTTListener> MQTTListener::getInstance() {
  return make_shared<MQTTListener>();
}

void MQTTListener::init() {}

void MQTTListener::startListening() {}

string MQTTListener::getHostName() {
    return hostName;
}

int MQTTListener::getPort() {
    return port;
}

string MQTTListener::getClientId() {
    return clientId;
}

string MQTTListener::getTopic() {
    return topic;
}
