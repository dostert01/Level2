#include "application.h"

namespace level2 {
    
    std::shared_ptr<Application> Application::getInstance(const std::string& configFilePath) {
        std::shared_ptr<Application> instance = std::make_shared<Application>();
        APP_CONTEXT.loadApplicationConfig(configFilePath);
        return instance;
    }
}
