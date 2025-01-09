#ifndef MQTT_LISTENER_H
#define MQTT_LISTENER_H

#include <memory>

#include "networklistener.h"

using namespace std;

class MQTTListener : public NetworkListener {
    public:
        MQTTListener() = default;
        static shared_ptr<MQTTListener> getInstance();
        void init();
        void startListening();
};

#endif //#define MQTT_LISTENER_H