#ifndef MQTT_LISTENER_H
#define MQTT_LISTENER_H

#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

#include "networklistener.h"
#include "mosquittowrapper.h"

using namespace std;
using namespace nlohmann::json_abi_v3_11_3;

class MQTTTopic {
  public:
    MQTTTopic(json jsonObject);
    string getName();
  private:
    string name;
};

class MQTTListener : public NetworkListener {
    public:
        MQTTListener() = default;
        MQTTListener(json jsonObject);
        static shared_ptr<MQTTListener> getInstance();
        void init(shared_ptr<PipelineFiFo> fifo);
        void startListening();
        string getHostName();
        int getPort();
        string getClientId();
        optional<string> getTopic(int index);
        vector<string> getTopicNames();
    private:
        string hostName = "none";
        int port = 1234;
        string clientId = "hallo";
        vector<shared_ptr<MQTTTopic>> topics;
        std::shared_ptr<MosquittoWrapper> mqtt;
};

#endif //#define MQTT_LISTENER_H