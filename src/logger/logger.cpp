#include <logger.h>

#include <iostream>
#include <memory>

namespace second_take {

Logger::Logger() {
  timeProvider = std::make_unique<DefaultTimeProvider>();
  std::unique_ptr<LoggingDestination> destination(
      new LoggingDestinationStdOut());
  loggingDestinations.push_back(std::move(destination));
}

Logger::~Logger() {}

Logger &Logger::getInstance() {
  static Logger instance;
  return instance;
}

Logger *Logger::getInstanceAsPointer() { return &getInstance(); }

bool operator==(const Logger &lhs, const Logger &rhs) {
  return (&lhs.getInstance() == &rhs.getInstance());
}

void Logger::trace(const std::string &message) {
  doLogging(LogLevel::LOG_LEVEL_TRACE, message);
}

void Logger::debug(const std::string &message) {
  doLogging(LogLevel::LOG_LEVEL_DEBUG, message);
}

void Logger::info(const std::string &message) {
  doLogging(LogLevel::LOG_LEVEL_INFO, message);
}

void Logger::warn(const std::string &message) {
  doLogging(LogLevel::LOG_LEVEL_WARN, message);
}

void Logger::error(const std::string &message) {
  doLogging(LogLevel::LOG_LEVEL_ERROR, message);
}

void Logger::fatal(const std::string &message) {
  doLogging(LogLevel::LOG_LEVEL_FATAL, message);
}

void Logger::doLogging(const LogLevel &logLevel, const std::string &message) {
  std::string timestamp = timeProvider->getTimeStampOfNow();
  for (const auto &currentLoggingDestination : loggingDestinations) {
    currentLoggingDestination->doLogging(logLevel, message, timestamp);
  }
}

}  // namespace second_take
