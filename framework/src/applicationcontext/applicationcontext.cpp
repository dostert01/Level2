#include <applicationcontext.h>
#include <logger.h>
#include <systemUtils.h>

#include <fstream>
#include <iostream>
#include <format>

#define LOGGER_CONFIG_PATH "logging/loggers"

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
    LOGGER.info(std::format("Loading app config from file: {}", configFilePath));
    std::ifstream jsonFile(configFilePath);
    jsonAppConfigData = json::parse(jsonFile);
    jsonFile.close();
    LOGGER.debug(std::format("Success loading app config from file: ", configFilePath));
  } catch (const std::exception& e) {
    LOGGER.error(e.what());
  }
}

void ApplicationContext::configureLogger() {
  auto loggerJson = findRecursiveInJsonTree(LOGGER_CONFIG_PATH);
  if(loggerJson.has_value()) {
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    addLoggingDestinationsFromJson2TheLogger(loggerJson, factory);
    LOGGER.debug(std::format("Logger has been configured from json: '{}'", loggerJson.value().dump()));
  }
}

void ApplicationContext::addLoggingDestinationsFromJson2TheLogger(std::optional<json> &loggerJson, LoggingDestinationFactory &factory)
{
  for (const auto &destinationConfig : loggerJson.value())
  {
    if(destinationConfig.contains("type")) {
      std::map<std::string, std::string> params;
      for (const auto &[key, value] : destinationConfig.items()) {
        params[key] = value;
      }
      LOGGER.addLoggingDestination(factory.createDestinationFromParamsMap(params));
    }
  }
}

std::optional<json> ApplicationContext::findRecursiveInJsonTree(const std::string& path) {
  return findRecursiveInJsonTree(jsonAppConfigData, path);
}

std::optional<json> ApplicationContext::findRecursiveInJsonTree(json objectAsJson, const std::string& path) {
  LOGGER.debug(std::format("searching for json objects with path: '{}'", path));
  std::vector<std::string> pathElements = StaticStringFunctions::splitString(path, PATH_SEPARATOR);
  json currentJson = objectAsJson;
  for (auto const& pathElement : pathElements) {
    if (currentJson.contains(pathElement)) {
      currentJson = currentJson[pathElement];
    } else {
      LOGGER.error(std::format("json std::string '{}' does not contain the required object or array named: '{}'",
        currentJson.dump(), pathElement));
      return std::nullopt;
    }
  }
  return currentJson;
}

}  // namespace level2