#include <fstream>
#include <iostream>
#include <format>

#include "logger.h"
#include "systemUtils.h"
#include "applicationcontext.h"

#define JSON_NAME_LOGGING_DESTINATION_CONFIG_PATH "logging/loggers"
#define JSON_NAME_LOGLEVEL_CONFIG_PATH "logging"
#define JSON_NAME_LOG_LEVEL "logLevel"

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

void ApplicationContext::resetAllMemberVariables() {
  jsonAppConfigData = nullptr;
  applicationDirectories = nullptr;
}

std::shared_ptr<ApplicationDirectories>& ApplicationContext::getApplicationDirectories() {
  return applicationDirectories;
}

void ApplicationContext::setApplicationDirectories(std::shared_ptr<ApplicationDirectories>& appDirectories) {
  applicationDirectories = appDirectories;
}

void ApplicationContext::loadApplicationConfig(const std::string& configFilePath) {
  resetAllMemberVariables();
  loadJsonConfig(configFilePath);
  configureLogger();
}

void ApplicationContext::loadJsonConfig(const std::string &configFilePath)
{
  try {
    LOGGER.info(std::format("Loading app config from file: {}", configFilePath));
    std::ifstream jsonFile(configFilePath);
    jsonAppConfigData = json::parse(jsonFile);
    jsonFile.close();
    LOGGER.debug(std::format("Success loading app config from file: ", configFilePath));
  }
  catch (const std::exception &e) {
    LOGGER.error(e.what());
  }
}

void ApplicationContext::configureLogger() {
  configureLoggingDestinations();
  configureLogLevel();
}

void ApplicationContext::configureLogLevel() {
  auto loglevelJson = findRecursiveInJsonTree(JSON_NAME_LOGLEVEL_CONFIG_PATH);
  if (loglevelEntryInJsonIsValid(loglevelJson)) {
    LOGGER.setMaxLogLevel(std::string(loglevelJson.value().at(JSON_NAME_LOG_LEVEL)));
  }
}

bool ApplicationContext::loglevelEntryInJsonIsValid(std::optional<nlohmann::json_abi_v3_11_3::json> &loglevelJson) {
  return loglevelJson.has_value() && (loglevelJson.value().contains(JSON_NAME_LOG_LEVEL)) && loglevelJson.value()[JSON_NAME_LOG_LEVEL].is_string();
}

void ApplicationContext::configureLoggingDestinations() {
  LOGGER.removeAllDestinations();
  addNewLoggingDestinations();
}

void ApplicationContext::addNewLoggingDestinations()
{
  LoggingDestinationFactory factory = LoggingDestinationFactory();
  auto loggerJson = findRecursiveInJsonTree(JSON_NAME_LOGGING_DESTINATION_CONFIG_PATH);
  if (loggerJson.has_value())
  {
    addLoggingDestinationsFromJson2TheLogger(loggerJson, factory);
    LOGGER.debug(std::format("Logger has been configured from json: '{}'", loggerJson.value().dump()));
  } else {
    LOGGER.addLoggingDestination(factory.createDestinationStdOut());
    LOGGER.warn("No logging destination was defines by application config. Hence, the default logging destination will be used.");
  }
}

void ApplicationContext::addLoggingDestinationsFromJson2TheLogger(std::optional<json> &loggerJson, LoggingDestinationFactory &factory) {
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