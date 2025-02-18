#pragma once

#include <vector>
#include <optional>
#include <string>

namespace level2 {
    namespace common {

        std::optional<std::string> readEnv(std::string variableName);
        std::optional<std::string> getCurrentWorkingDirectory();
        std::vector<std::string> splitString(std::string toSplit, std::string separator);
        std::string replaceEnvVariablesInPath(std::string path);
    }
}