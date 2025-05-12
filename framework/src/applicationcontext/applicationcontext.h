#pragma once

#include <logger.h>
#include <applicationDirectories.h>

#include <nlohmann/json.hpp>
#include <optional>
#include <shared_mutex>
#include <string>
#include <vector>

#define APP_CONTEXT ApplicationContext::getInstance()

using namespace nlohmann::json_abi_v3_11_3;

namespace level2 {

class ApplicationContext {
 public:
  static ApplicationContext &getInstance();
  static ApplicationContext *getInstanceAsPointer();
  friend bool operator==(const ApplicationContext &lhs, const ApplicationContext &rhs);
  void loadApplicationConfig(const std::string &configFilePath);
  std::optional<json> findRecursiveInJsonTree(const std::string& path);
  std::optional<json> findRecursiveInJsonTree(json objectAsJson, const std::string& path);
  std::shared_ptr<ApplicationDirectories> getApplicationDirectories();

  template <typename T, typename... Args>
  void createSingleObject(std::vector<std::shared_ptr<T>> &returnValue, json &jsonObject, Args &&...args) const {
    LOGGER.debug("jsonObject to create an object from: " + jsonObject.dump());
    try {
      returnValue.emplace_back(std::make_shared<T>(jsonObject, std::forward<Args>(args)...));
    } catch (const std::exception &e) {
      LOGGER.error(
          std::string("Exception occurred during parsing json to an object. The "
                  "corresponding object will be missing! Errormessage: ") + e.what());
      LOGGER.warn("default constructor will be used instead of failed initialization from json. "
        "Maybe the provided default values might be incorrect or unexpected in the context of "
        "the application. Try fixing your json input!");
      returnValue.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
    }      
  }

  template <typename T, typename... Args>
  std::vector<std::shared_ptr<T>> createObjectsFromJson(json objectAsJson, std::string path, Args &&...args) {
    std::vector<std::shared_ptr<T>> returnValue;
    if (std::optional<json> toBeCreatedFrom = findRecursiveInJsonTree(objectAsJson, path); toBeCreatedFrom.has_value()) {
      json currentJson = toBeCreatedFrom.value();
      if (currentJson.is_array()) {
        for (auto &jsonObject : currentJson) {
          createSingleObject(returnValue, jsonObject, std::forward<Args>(args)...);
        }
      } else if (currentJson.is_object()) {
          createSingleObject(returnValue, currentJson, std::forward<Args>(args)...);
      }
    }
    return std::move(returnValue);
  };

  template <typename T, typename... Args>
  std::vector<std::shared_ptr<T>> createObjectsFromAppConfigJson(std::string path, Args &&...args) {
    return std::move(createObjectsFromJson<T>(jsonAppConfigData, path, std::forward<Args>(args)...));
  };

 private:
  ApplicationContext() = default;
  json jsonAppConfigData;
  std::shared_ptr<ApplicationDirectories> applicationDirectories;
  void configureApplicationDirectories();
  void resetAllMemberVariables();
  void loadJsonConfig(const std::string &configFilePath);
  void configureLoggingDestinations();
  void addNewLoggingDestinations();
  void configureLogger();
  void configureLogLevel();
  bool loglevelEntryInJsonIsValid(std::optional<json> &loglevelJson);
  void addLoggingDestinationsFromJson2TheLogger(std::optional<json> &loggerJson, LoggingDestinationFactory &factory);
};

}  // namespace level2
