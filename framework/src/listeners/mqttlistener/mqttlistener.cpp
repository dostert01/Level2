#include "mqttlistener.h"

using namespace std;

shared_ptr<MQTTListener> MQTTListener::getInstance() {
    return make_shared<MQTTListener>();
}

void MQTTListener::init() {

}

void MQTTListener::startListening() {

}