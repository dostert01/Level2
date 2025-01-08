#ifndef MOSQUITTO_WRAPPER_H
#define MOSQUITTO_WRAPPER_H

#define MQTT_CONNECTOR_PARAM_NAME_HOSTNAME "hostName"
#define MQTT_CONNECTOR_PARAM_NAME_PORT "port"
#define MQTT_CONNECTOR_PARAM_NAME_CLIENT_ID "clientId"
#define MQTT_CONNECTOR_PARAM_NAME_TOPIC "topic"
#define PAYLOAD_NAME_MQTT_SEND_TEXT_DATA "MQTT_SEND_TEXT_DATA"

#include <memory>
#include <shared_mutex>

using namespace std;

class MosquittoWrapper {
 public:
  static std::shared_mutex mqttConnectorInitMutex;
  static shared_ptr<MosquittoWrapper> getInstance(string hostName, int port,
                                                  string clientId,
                                                  string topic);
  MosquittoWrapper() = default;
  ~MosquittoWrapper();
  bool isInitComplete();
  void sendData(string payloadString);

 private:
  string hostName;
  int port;
  string clientId;
  string topic;
  bool initComplete;
  bool isConnected;
  struct mosquitto *mosquittoHandle;
  void init(string hostName, int port, string clientId, string topic);
  void initParams(std::string &hostName, int port, std::string &clientId,
                  std::string &topic);
  void initMosquittoLib();
  void initMosquittoConnection();
};

#endif  // MOSQUITTO_WRAPPER_H