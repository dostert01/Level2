#pragma once

#include <optional>
#include <string>
#include <vector>
#include <utility>

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR "/"
#endif

namespace level2 {
class StaticStringFunctions {
 public:
  static std::optional<std::string> readEnv(std::string variableName);
  static std::optional<std::string> getCurrentWorkingDirectory();
  static std::vector<std::string> splitString(std::string toSplit, std::string separator);
  static std::string replaceEnvVariablesInPath(std::string path);
};

class StaticFileFunctions {
    public:
        static std::pair<bool, std::string> createDirectory(std::string directoryName);
};

}  // namespace level2