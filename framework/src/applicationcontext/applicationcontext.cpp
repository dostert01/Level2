#include <applicationcontext.h>
#include <logger.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

namespace event_forge {

#define DEFAULT_BUFFER_SIZE 2048

ApplicationContext& ApplicationContext::getInstance() {
  static ApplicationContext instance;
  return instance;
}

ApplicationContext* ApplicationContext::getInstanceAsPointer() {
  return &getInstance();
}

bool operator==(const ApplicationContext& lhs, const ApplicationContext& rhs) {
  return (&lhs.getInstance() == &rhs.getInstance());
}

std::optional<std::string> ApplicationContext::readEnv(std::string variableName) {
  char* value = secure_getenv(variableName.c_str());
  if (value != NULL) {
    return std::string(value);
  } else {
    return std::nullopt;
  }
}

std::optional<std::string> ApplicationContext::getCurrentWorkingDirectory() {
  char* value = getcwd(NULL, 0);
  if (value != NULL) {
    std::string returnValue = std::string(value);
    free(value);
    return returnValue;
  } else {
    char buffer[DEFAULT_BUFFER_SIZE];
    char* errorText = strerror_r(errno, buffer, DEFAULT_BUFFER_SIZE);
    LOGGER.warn(std::string(errorText));
    return std::nullopt;
  }
}

void ApplicationContext::loadApplicationConfig(const std::string& configFilePath) {
  try {
    LOGGER.info("Loading app config from file: " + configFilePath);
    std::ifstream jsonFile(configFilePath);
    jsonAppConfigData = json::parse(jsonFile);
    jsonFile.close();
    LOGGER.debug("Success loading app config from file: " + configFilePath);
  } catch (const std::exception& e) {
    LOGGER.error(e.what());
  }
}

std::vector<std::string> ApplicationContext::splitString(std::string toSplit, std::string separator) {
    std::vector<std::string> result;
    int start = 0;
    int found = toSplit.find(separator, start);
    while(found != std::string::npos) {
        result.push_back(toSplit.substr(start, found - start));
        start += (found - start) + separator.length();
        found = toSplit.find(separator, start);
    }
    result.push_back(toSplit.substr(start));
    return result;
}

  std::optional<json> ApplicationContext::findRecursiveInJsonTree(std::string path) {
    return findRecursiveInJsonTree(jsonAppConfigData, path);
  }

  std::optional<json> ApplicationContext::findRecursiveInJsonTree(json objectAsJson, std::string path) {
    LOGGER.debug("searching for json objects with path: '" + path + "'");
    std::vector<std::string> pathElements = splitString(path, "/");
    json currentJson = objectAsJson;
    for (auto pathElement : pathElements) {
      if (currentJson.contains(pathElement)) {
        currentJson = currentJson[pathElement];
      } else {
        LOGGER.error(
            "json std::string '" + currentJson.dump() +
            "' does not contain the required object or array named: '" +
            pathElement + "'");
        return std::nullopt;
      }
    }
    return currentJson;
  }

}  // namespace event_forge