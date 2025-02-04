#ifndef MQTT_LISTENER_H
#define MQTT_LISTENER_H

#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

#include "networklistener.h"
#include "mosquittowrapper.h"

using namespace nlohmann::json_abi_v3_11_3;

namespace level2 {

class MQTTTopic {
  public:
    MQTTTopic(json jsonObject);
    std::string getName();
  private:
    std::string name;
};

class MQTTListener : public NetworkListener {
    public:
        MQTTListener() = default;
        MQTTListener(json jsonObject);
        static std::shared_ptr<MQTTListener> getInstance();
        void init(std::shared_ptr<PipelineFiFo> fifo);
        void startListening();
        std::string getHostName();
        int getPort();
        std::string getClientId();
        std::optional<std::string> getTopic(int index);
        std::vector<std::string> getTopicNames();
    private:
        std::string hostName = "none";
        int port = 1234;
        std::string clientId = "hallo";
        std::vector<std::shared_ptr<MQTTTopic>> topics;
        std::shared_ptr<MosquittoWrapper> mqtt;
};

} // namespace level2
#endif //#define MQTT_LISTENER_H