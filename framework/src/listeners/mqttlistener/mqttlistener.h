#ifndef MQTT_LISTENER_H
#define MQTT_LISTENER_H

#include <memory>
#include <nlohmann/json.hpp>

#include "networklistener.h"

using namespace std;
using namespace nlohmann::json_abi_v3_11_3;

class MQTTListener : public NetworkListener {
    public:
        MQTTListener() = default;
        MQTTListener(json jsonObject);
        static shared_ptr<MQTTListener> getInstance();
        void init();
        void startListening();
        string getHostName();
        int getPort();
        string getClientId();
        string getTopic();
    private:
        string hostName = "none";
        int port = 1234;
        string clientId = "hallo";
        string topic = "test/topic";
};

#endif //#define MQTT_LISTENER_H