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
  friend bool operator==(const ApplicationContext &lhs,
                         const ApplicationContext &rhs);
  std::optional<std::string> readEnv(std::string variableName);
  std::optional<std::string> getCurrentWorkingDirectory();
  void loadApplicationConfig(const std::string &configFilePath);
  std::vector<std::string> splitString(std::string toSplit, std::string separator);
  std::optional<json> findRecursiveInJsonTree(std::string path);
  std::optional<json> findRecursiveInJsonTree(json objectAsJson, std::string path);

  template <typename T, typename... Args>
  std::vector<std::shared_ptr<T>> createObjectsFromJson(json objectAsJson, std::string path, Args &&...args) {
    std::vector<std::shared_ptr<T>> returnValue;
    std::optional<json> toBeCreatedFrom = findRecursiveInJsonTree(objectAsJson, path);
    if (toBeCreatedFrom.has_value() && toBeCreatedFrom.value().is_array()) {
      for (auto &jsonObject : toBeCreatedFrom.value()) {
        LOGGER.debug("jsonObject to create an object from: " + jsonObject.dump());
        try {
          returnValue.emplace_back(std::make_shared<T>(jsonObject, std::forward<Args>(args)...));
        } catch (const std::exception &e) {
          LOGGER.error(
              std::string("Exception occurred during parsing json to an object. The "
                     "corresponding object will be missing! Errormessage: ") + e.what());
        }
      }
    } else if (toBeCreatedFrom.has_value() && toBeCreatedFrom.value().is_object()) {
        try {
          returnValue.emplace_back(std::make_shared<T>(toBeCreatedFrom.value(), std::forward<Args>(args)...));
        } catch (const std::exception &e) {
          LOGGER.error(
              std::string("Exception occurred during parsing json to an object. The "
                     "corresponding object will be missing! Errormessage: ") + e.what());
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
};

}  // namespace level2
