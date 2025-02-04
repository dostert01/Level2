#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <shared_mutex>

#include "loggingdestinations.h"
#include "loglevels.h"
#include "timeproviders.h"

#define LOGGER Logger::getInstance()

namespace level2 {

class Logger {
 public:
  virtual ~Logger();
  static Logger &getInstance();
  static Logger *getInstanceAsPointer();
  friend bool operator==(const Logger &lhs, const Logger &rhs);
  void trace(const std::string &message);
  void debug(const std::string &message);
  void info(const std::string &message);
  void warn(const std::string &message);
  void error(const std::string &message);
  void fatal(const std::string &message);
  void removeAllDestinations();
  size_t getCountOfLoggingDestinations();
  void addLoggingDestination(std::unique_ptr<LoggingDestination> destination);
  void setLoggingDestination(std::unique_ptr<LoggingDestination> destination);
  void setMaxLogLevel(LogLevel logLevel);

 private:
  Logger();
  std::unique_ptr<TimeProvider> timeProvider;
  std::vector<std::unique_ptr<LoggingDestination>> loggingDestinations;
  LogLevel maxLogLevel;
  LogLevelStringMapper logLevelStringMapper;
  std::shared_mutex mutex;

  void doLogging(const LogLevel &logLevel, const std::string &message);
  bool logThisLevel(const LogLevel &logLevel);
  void checkLogLevelInEnvironment();
};

}  // namespace level2
