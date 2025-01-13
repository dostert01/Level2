#ifndef MOSQUITTO_WRAPPER_H
#define MOSQUITTO_WRAPPER_H

#define MQTT_CONNECTOR_PARAM_NAME_HOSTNAME "hostName"
#define MQTT_CONNECTOR_PARAM_NAME_PORT "port"
#define MQTT_CONNECTOR_PARAM_NAME_CLIENT_ID "clientId"
#define MQTT_CONNECTOR_PARAM_NAME_TOPIC "topic"
#define PAYLOAD_NAME_MQTT_SEND_TEXT_DATA "MQTT_SEND_TEXT_DATA"

#include <memory>
#include <vector>
#include <shared_mutex>

using namespace std;

class MosquittoWrapper {
 public:
  static std::shared_mutex mqttConnectorInitMutex;
  static shared_ptr<MosquittoWrapper> getInstance(string hostName,
                                                  int port,
                                                  string clientId,
                                                  string topic);
  static shared_ptr<MosquittoWrapper> getInstance(string hostName,
                                                  int port,
                                                  string clientId,
                                                  vector<string> topics);
  MosquittoWrapper() = default;
  ~MosquittoWrapper();
  bool isInitComplete();
  void sendData(string payloadString);
  void startListening();
  bool isListening();

 private:
  string hostName;
  int port;
  string clientId;
  vector<string> topics;
  bool initComplete;
  bool connected;
  bool listening;
  struct mosquitto *mosquittoHandle;
  void init(string hostName, int port, string clientId, vector<string>& topics);
  void initParams(string &hostName, int port, string &clientId, vector<string> topics);
  void initMosquittoLib();
  void initMosquittoConnection();
  void disconnectFromBroker();
  void logMosquittoMessage(int mosquittoReturnCode, string sucessMessage, string errorMessage);
};

#endif  // MOSQUITTO_WRAPPER_H