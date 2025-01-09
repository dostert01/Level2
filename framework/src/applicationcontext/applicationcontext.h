#ifndef SECOND_TAKE_APP_CTX
#define SECOND_TAKE_APP_CTX

#include <string>
#include <shared_mutex>
#include <optional>

#define APP_CONTEXT ApplicationContext::getInstance()

using namespace std;

namespace event_forge {

class ApplicationContext {
 public:
  static ApplicationContext &getInstance();
  static ApplicationContext *getInstanceAsPointer();
  friend bool operator==(const ApplicationContext &lhs, const ApplicationContext &rhs);
  optional<string> readEnv(string variableName);
  optional<string> getCurrentWorkingDirectory();
  void loadApplicationConfig(const std::string& configFilePath);
 private:
  ApplicationContext() = default;
};

}  // namespace event_forge

#endif  // SECOND_TAKE_APP_CTX