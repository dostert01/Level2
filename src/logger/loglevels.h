#ifndef LOG_LEVELS_H
#define LOG_LEVELS_H

#include <map>
#include <string>
#include <optional>

namespace second_take {

#define LOG_PREFIX_TRACE "TRACE"
#define LOG_PREFIX_DEBUG "DEBUG"
#define LOG_PREFIX_INFO "INFO"
#define LOG_PREFIX_WARN "WARN"
#define LOG_PREFIX_ERROR "ERROR"
#define LOG_PREFIX_FATAL "FATAL"
#define LOG_PREFIX_NO_LOGGING "NO_LOGGING"

enum class LogLevel {
  LOG_LEVEL_TRACE = 6,
  LOG_LEVEL_DEBUG = 5,
  LOG_LEVEL_INFO = 4,
  LOG_LEVEL_WARN = 3,
  LOG_LEVEL_ERROR = 2,
  LOG_LEVEL_FATAL = 1,
  LOG_LEVEL_NO_LOGGING = 0
};

class LogLevelStringMapper {
 public:
  LogLevelStringMapper();
  std::optional<LogLevel> string2LogLevel(const std::string &levelString) const;
  std::optional<std::string> logLevel2String(const LogLevel &logLevel) const ;

 private:
  std::map<LogLevel, std::string> logLevels;
};

}  // namespace second_take

#endif  // LOG_LEVELS_H