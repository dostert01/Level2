#include <applicationcontext.h>
#include <logger.h>
#include <stdlib.h>
#include <string>
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

optional<string> ApplicationContext::readEnv(string variableName) {
  char* value = secure_getenv(variableName.c_str());
  if (value != NULL) {
    return string(value);
  } else {
    return nullopt;
  }
}

optional<string> ApplicationContext::getCurrentWorkingDirectory() {
  char* value = getcwd(NULL, 0);
  if (value != NULL) {
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
    LOGGER.info("Loading app config from file: " + configFilePath);
    std::ifstream jsonFile(configFilePath);
    jsonAppConfigData = json::parse(jsonFile);
    jsonFile.close();
    LOGGER.debug("Success loading app config from file: " + configFilePath);
  } catch (const std::exception& e) {
    LOGGER.error(e.what());
  }
}

vector<string> ApplicationContext::splitString(string toSplit, string separator) {
    vector<string> result;
    int start = 0;
    int found = toSplit.find(separator, start);
    while(found != string::npos) {
        result.push_back(toSplit.substr(start, found - start));
        start += (found - start) + separator.length();
        found = toSplit.find(separator, start);
    }
    result.push_back(toSplit.substr(start));
    return result;
}

  optional<json> ApplicationContext::findRecursiveInJsonTree(string path) {
    return findRecursiveInJsonTree(jsonAppConfigData, path);
  }

  optional<json> ApplicationContext::findRecursiveInJsonTree(json objectAsJson, string path) {
    LOGGER.debug("searching for json objects with path: '" + path + "'");
    vector<string> pathElements = splitString(path, "/");
    json currentJson = objectAsJson;
    for (auto pathElement : pathElements) {
      if (currentJson.contains(pathElement)) {
        currentJson = currentJson[pathElement];
      } else {
        LOGGER.error(
            "json string '" + currentJson.dump() +
            "' does not contain the required object or array named: '" +
            pathElement + "'");
        return nullopt;
      }
    }
    return currentJson;
  }

}  // namespace event_forge