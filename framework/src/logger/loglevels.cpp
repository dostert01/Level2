#include "loglevels.h"

namespace level2 {

LogLevelStringMapper::LogLevelStringMapper() {
  logLevels = {{LogLevel::LOG_LEVEL_NO_LOGGING, LOG_PREFIX_NO_LOGGING},
               {LogLevel::LOG_LEVEL_FATAL, LOG_PREFIX_FATAL},
               {LogLevel::LOG_LEVEL_ERROR, LOG_PREFIX_ERROR},
               {LogLevel::LOG_LEVEL_WARN, LOG_PREFIX_WARN},
               {LogLevel::LOG_LEVEL_INFO, LOG_PREFIX_INFO},
               {LogLevel::LOG_LEVEL_DEBUG, LOG_PREFIX_DEBUG},
               {LogLevel::LOG_LEVEL_TRACE, LOG_PREFIX_TRACE}};
}

std::optional<LogLevel> LogLevelStringMapper::string2LogLevel(
    const std::string &levelString) const {
  for (const auto &[levelAsEnum, levelAsString] : logLevels) {
    if (levelAsString == levelString) {
      return levelAsEnum;
    }
  }
  return std::nullopt;
}

std::optional<std::string> LogLevelStringMapper::logLevel2String(const LogLevel &logLevel) const {
    try {
        return logLevels.at(logLevel);
    } catch (...) {
        return std::nullopt;
    }
}

}  // namespace level2
