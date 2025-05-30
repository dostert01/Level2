#include <applicationcontext.h>
#include <logger.h>
#include <systemUtils.h>

#include <fstream>
#include <iostream>

namespace level2 {

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

void ApplicationContext::configureLogger() {
  LOGGER.removeAllDestinations();
  
}

std::optional<json> ApplicationContext::findRecursiveInJsonTree(std::string& path) {
  return findRecursiveInJsonTree(jsonAppConfigData, path);
}

std::optional<json> ApplicationContext::findRecursiveInJsonTree(json objectAsJson, std::string& path) {
  LOGGER.debug("searching for json objects with path: '" + path + "'");
  std::vector<std::string> pathElements = StaticStringFunctions::splitString(path, PATH_SEPARATOR);
  json currentJson = objectAsJson;
  for (auto const& pathElement : pathElements) {
    if (currentJson.contains(pathElement)) {
      currentJson = currentJson[pathElement];
    } else {
      LOGGER.error("json std::string '" + currentJson.dump() +
                   "' does not contain the required object or array named: '" +
                   pathElement + "'");
      return std::nullopt;
    }
  }
  return currentJson;
}

}  // namespace level2