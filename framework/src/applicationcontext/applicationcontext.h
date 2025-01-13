#ifndef SECOND_TAKE_APP_CTX
#define SECOND_TAKE_APP_CTX

#include <logger.h>

#include <nlohmann/json.hpp>
#include <optional>
#include <shared_mutex>
#include <string>
#include <vector>

#define APP_CONTEXT ApplicationContext::getInstance()

using namespace nlohmann::json_abi_v3_11_3;
using namespace std;

namespace event_forge {

class ApplicationContext {
 public:
  static ApplicationContext &getInstance();
  static ApplicationContext *getInstanceAsPointer();
  friend bool operator==(const ApplicationContext &lhs,
                         const ApplicationContext &rhs);
  optional<string> readEnv(string variableName);
  optional<string> getCurrentWorkingDirectory();
  void loadApplicationConfig(const std::string &configFilePath);
  vector<string> splitString(string toSplit, string separator);
  optional<json> findRecursiveInJsonTree(string path);
  optional<json> findRecursiveInJsonTree(json objectAsJson, string path);

  template <typename T, typename... Args>
  vector<shared_ptr<T>> createObjectsFromJson(json objectAsJson, string path, Args &&...args) {
    vector<shared_ptr<T>> returnValue;
    optional<json> toBeCreatedFrom = findRecursiveInJsonTree(objectAsJson, path);
    if (toBeCreatedFrom.has_value()) {
      for (auto &jsonObject : toBeCreatedFrom.value()) {
        LOGGER.debug("jsonObject to create an object from: " + jsonObject.dump());
        try {
          returnValue.emplace_back(make_shared<T>(jsonObject, std::forward<Args>(args)...));
        } catch (const exception &e) {
          LOGGER.error(
              string("Exception occurred during parsing json to an object. The "
                     "corresponding object will be missing! Errormessage: ") + e.what());
        }
      }
    }
    return std::move(returnValue);
  };

  template <typename T, typename... Args>
  vector<shared_ptr<T>> createObjectsFromAppConfigJson(string path, Args &&...args) {
    return std::move(createObjectsFromJson<T>(jsonAppConfigData, path, std::forward<Args>(args)...));
  };

 private:
  ApplicationContext() = default;
  json jsonAppConfigData;
};

}  // namespace event_forge

#endif  // SECOND_TAKE_APP_CTX