#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <nlohmann/json.hpp>
#include <applicationcontext.h>
#include <logger.h>

namespace event_forge {

#define DEFAULT_BUFFER_SIZE 2048

ApplicationContext &ApplicationContext::getInstance() {
  static ApplicationContext instance;
  return instance;
}

ApplicationContext *ApplicationContext::getInstanceAsPointer() {
    return &getInstance(); 
}

bool operator==(const ApplicationContext &lhs, const ApplicationContext &rhs) {
  return (&lhs.getInstance() == &rhs.getInstance());
}

optional<string> ApplicationContext::readEnv(string variableName) {
    char* value = secure_getenv(variableName.c_str());
    if(value != NULL) {
        return string(value);
    } else {
        return nullopt;
    }
}

optional<string> ApplicationContext::getCurrentWorkingDirectory() {
    char* value = getcwd(NULL, 0);
    if(value != NULL) {
        string returnValue = string(value);
        free(value);
        return returnValue;
    } else {
        char buffer[DEFAULT_BUFFER_SIZE];
        char* errorText = strerror_r(errno, buffer, DEFAULT_BUFFER_SIZE);
        LOGGER.warn(string(errorText));
        return nullopt;
    }
}


void ApplicationContext::loadApplicationConfig(const std::string& configFilePath) {
  try {
    std::ifstream jsonFile(configFilePath);
    json jsonData = json::parse(jsonFile);
    jsonFile.close();
    loadHeaderData(jsonData);
    loadPipelines(jsonData);
    } catch (const std::exception& e) {
        throw;
    }
}

}