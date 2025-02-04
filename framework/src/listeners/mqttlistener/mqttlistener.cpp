#include "mqttlistener.h"
#include "applicationcontext.h"

namespace level2 {

MQTTListener::MQTTListener(json jsonObject) {
  hostName = jsonObject["hostName"];
  port = jsonObject["port"];
  clientId = jsonObject["clientId"];
  topics = APP_CONTEXT.createObjectsFromJson<MQTTTopic>(jsonObject, "topics");
};

std::shared_ptr<MQTTListener> MQTTListener::getInstance() {
  return std::make_shared<MQTTListener>();
}

void MQTTListener::init(std::shared_ptr<PipelineFiFo> fifo) {
    NetworkListener::init(fifo);
    MosquittoWrapper::getInstance(hostName, port, clientId, getTopicNames());
    mqtt = MosquittoWrapper::getInstance(hostName, port, clientId, getTopicNames());
    initComplete = mqtt->isInitComplete();
}

void MQTTListener::startListening() {
    mqtt->startListening(pipelineFifo);
    setIsListening(mqtt->isListening());
}

std::string MQTTListener::getHostName() {
    return hostName;
}

int MQTTListener::getPort() {
    return port;
}

std::string MQTTListener::getClientId() {
    return clientId;
}

std::optional<std::string> MQTTListener::getTopic(int index) {
    if((index > -1) && (index < topics.size())) {
        return topics[index].get()->getName();
    } else {
        return std::nullopt;
    }
}

std::vector<std::string> MQTTListener::getTopicNames() {
    std::vector<std::string> returnValue;
    for(auto topic : topics) {
        returnValue.push_back(topic.get()->getName());
    }
    return returnValue;
}

// ----------------------------------------------------------------------------
MQTTTopic::MQTTTopic(json jsonObject) {
  name = jsonObject["name"];
}

std::string MQTTTopic::getName() {
  return name;
}
} // namespace level2