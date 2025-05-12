#include <applicationcontext.h>
#include <logger.h>
#include <systemUtils.h>

#include <fstream>
#include <iostream>
#include <format>

#define LOGGING_DESTINATION_CONFIG_PATH "logging/loggers"
#define LOGLEVEL_CONFIG_PATH "logging"
#define ENTITY_NAME_LOG_LEVEL "logLevel"
#define APPLICATION_DIRECTORIES "applicationDirectories"

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
  applicationDirectories = nullptr;
  jsonAppConfigData = nullptr;
}

void ApplicationContext::loadApplicationConfig(const std::string& configFilePath) {
  resetAllMemberVariables();
  loadJsonConfig(configFilePath);
  configureLogger();
  configureApplicationDirectories();
}

std::shared_ptr<ApplicationDirectories> ApplicationContext::getApplicationDirectories() {
  return applicationDirectories;
}

void ApplicationContext::configureApplicationDirectories()
{
  auto dirs = APP_CONTEXT.createObjectsFromAppConfigJson<ApplicationDirectories>(APPLICATION_DIRECTORIES);
  if (dirs.size() > 0) {
    applicationDirectories = dirs[0];
  } else {
    LOGGER.warn(std::format("application configuration does not contain an unambiguous definition for {}. Falling back to defaults.",
      std::string(APPLICATION_DIRECTORIES)));
    applicationDirectories = std::make_shared<ApplicationDirectories>();
  }
  applicationDirectories->createApplicationDirectories();
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
  auto loglevelJson = findRecursiveInJsonTree(LOGLEVEL_CONFIG_PATH);
  if (loglevelEntryInJsonIsValid(loglevelJson)) {
    LOGGER.setMaxLogLevel(std::string(loglevelJson.value().at(ENTITY_NAME_LOG_LEVEL)));
  }
}

bool ApplicationContext::loglevelEntryInJsonIsValid(std::optional<nlohmann::json_abi_v3_11_3::json> &loglevelJson) {
  return loglevelJson.has_value() && (loglevelJson.value().contains(ENTITY_NAME_LOG_LEVEL)) && loglevelJson.value()[ENTITY_NAME_LOG_LEVEL].is_string();
}

void ApplicationContext::configureLoggingDestinations() {
  LOGGER.removeAllDestinations();
  addNewLoggingDestinations();
}

void ApplicationContext::addNewLoggingDestinations()
{
  auto loggerJson = findRecursiveInJsonTree(LOGGING_DESTINATION_CONFIG_PATH);
  if (loggerJson.has_value())
  {
    LoggingDestinationFactory factory = LoggingDestinationFactory();
    addLoggingDestinationsFromJson2TheLogger(loggerJson, factory);
    LOGGER.debug(std::format("Logger has been configured from json: '{}'", loggerJson.value().dump()));
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