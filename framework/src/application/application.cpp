#include <format>
#include <memory>
#include "application.h"

#define JSON_NAME_HTTP_LISTENERS "Listeners/HTTPListeners"
#define JSON_NAME_MQTT_LISTENERS "Listeners/MQTTListeners"
#define JSON_NAME_FILE_LISTENERS "Listeners/FileListeners"
#define JSON_NAME_APPLICATION_DIRECTORIES "applicationDirectories"

namespace level2 {
    
    Application::Application(const std::string& configFilePath) {
        APP_CONTEXT.loadApplicationConfig(configFilePath);
        configureApplicationDirectories();
        loadListeners();
        loadProcesses();
    }

    void Application::loadProcesses() {
        const std::filesystem::path processesJsonDirectoryPath(getApplicationDirectories()->getProcessesDir());
        for(auto const& file : std::filesystem::directory_iterator(processesJsonDirectoryPath) ) {
            auto processor = PipeLineProcessor::getInstance(file.path());
            if(processor.has_value()) {
                processes[processor.value()->getProcessName()] = processor.value();
            }
        }
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

    std::vector<std::shared_ptr<Listener>>& Application::getListeners() {
        return listeners;
    }

    std::map<std::string, std::shared_ptr<PipeLineProcessor>>& Application::getProcesses() {
        return processes;
    }

    std::shared_ptr<ApplicationDirectories>& Application::getApplicationDirectories() {
        return applicationDirectories;
    }
  
    void Application::configureApplicationDirectories()
    {
        initApplicationDirectories();
        APP_CONTEXT.setApplicationDirectories(getApplicationDirectories());
    }

    void Application::initApplicationDirectories() {
        auto dirs = APP_CONTEXT.createObjectsFromAppConfigJson<ApplicationDirectories>(JSON_NAME_APPLICATION_DIRECTORIES);
        if (dirs.size() > 0) {
            applicationDirectories = dirs[0];
        }
        else {
            LOGGER.warn(std::format("application configuration does not contain an unambiguous definition for {}. Falling back to defaults.",
                                    std::string(JSON_NAME_APPLICATION_DIRECTORIES)));
            applicationDirectories = std::make_shared<ApplicationDirectories>();
        }
        applicationDirectories->createApplicationDirectories();
    }
}
