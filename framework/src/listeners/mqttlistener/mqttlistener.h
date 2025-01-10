#ifndef MQTT_LISTENER_H
#define MQTT_LISTENER_H

#include <memory>
#include <nlohmann/json.hpp>

#include "networklistener.h"

using namespace std;

class MQTTListener : public NetworkListener {
    public:
        MQTTListener() = default;
        MQTTListener(MQTTListener& other){
            hostName = other.hostName;
            port = other.port;
            clientId = other.clientId;
            topic = other.topic;
        };
        static shared_ptr<MQTTListener> getInstance();
        void init();
        void startListening();
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(MQTTListener, hostName, port, clientId, topic)
    public:
        string hostName = "none";
        int port = 1234;
        string clientId = "hallo";
        string topic = "test/topic";
};

#endif //#define MQTT_LISTENER_H