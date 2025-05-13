#include <format>
#include <memory>
#include "application.h"

#define JSON_NAME_HTTP_LISTENERS "Listeners/HTTPListeners"
#define JSON_NAME_MQTT_LISTENERS "Listeners/MQTTListeners"
#define JSON_NAME_FILE_LISTENERS "Listeners/FileListeners"

namespace level2 {
    
    Application::Application(const std::string& configFilePath) {
        APP_CONTEXT.loadApplicationConfig(configFilePath);
        loadListeners();
    }

    std::shared_ptr<Application> Application::getInstance(const std::string& configFilePath) {
        std::shared_ptr<Application> instance = std::make_shared<Application>(configFilePath);
        return instance;
    }

    void Application::loadListeners() {
        addListeners<HTTPListener>(JSON_NAME_HTTP_LISTENERS);
        addListeners<FileListener>(JSON_NAME_FILE_LISTENERS);
        addListeners<MQTTListener>(JSON_NAME_MQTT_LISTENERS);
    }

    std::vector<std::shared_ptr<Listener>> Application::getListeners() {
        return listeners;
    }
}
