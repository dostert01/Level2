#include <stdlib.h>
#include <string.h>
#include <systemUtils.h>
#include <unistd.h>

namespace level2 {

std::optional<std::string> StaticStringFunctions::readEnv(
    std::string variableName) {
  char* value = secure_getenv(variableName.c_str());
  if (value != NULL) {
    return std::string(value);
  } else {
    return std::nullopt;
  }
}

std::optional<std::string> StaticStringFunctions::getCurrentWorkingDirectory() {
  char* value = getcwd(NULL, 0);
  if (value != NULL) {
    std::string returnValue = std::string(value);
    free(value);
    return returnValue;
  } else {
    return std::nullopt;
  }
}

std::vector<std::string> StaticStringFunctions::splitString(
    std::string toSplit, std::string separator) {
  std::vector<std::string> result;
  int start = 0;
  if (toSplit.ends_with(separator))
    toSplit = toSplit.substr(0, toSplit.length() - separator.length());
  int found = toSplit.find(separator, start);
  while (found != std::string::npos) {
    if (found != 0) {
      result.push_back(toSplit.substr(start, found - start));
    }
    start += (found - start) + separator.length();
    found = toSplit.find(separator, start);
  }
  result.push_back(toSplit.substr(start));
  return result;
}

std::string StaticStringFunctions::replaceEnvVariablesInPath(std::string path) {
  auto pathSections = StaticStringFunctions::splitString(path, PATH_SEPARATOR);
  path = "";
  for (size_t i = 0; i < pathSections.size(); i++) {
    if (pathSections[i].starts_with("$")) {
      char* variableName = (char*)malloc(pathSections[i].length());
      memset(variableName, 0, pathSections[i].length());
      sprintf(variableName, "%s", pathSections[i].c_str() + 1);
      char* envValue = secure_getenv(variableName);
      if (envValue) {
        pathSections[i] = envValue;
      }
      free(variableName);
      variableName = NULL;
    }
    path.append(pathSections[i].append(PATH_SEPARATOR));
  }
  path.pop_back();
  return path;
}

}  // namespace level2