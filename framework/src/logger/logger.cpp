#include <logger.h>

#include <iostream>
#include <memory>

namespace second_take {

Logger::Logger() {
  logLevelStringMapper = LogLevelStringMapper();
  setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
  timeProvider = std::make_unique<DefaultTimeProvider>();
  LoggingDestinationFactory destinationFactory = LoggingDestinationFactory();
  addLoggingDestination(destinationFactory.createDestinationStdOut());
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

void Logger::checkLogLevelInEnvironment() {
  const char* maxLogLevelFromEnv = getenv(MAX_LOG_LEVEL_ENV_VAR_NAME);
  if(maxLogLevelFromEnv != NULL) {
    std::optional<LogLevel> level = logLevelStringMapper.string2LogLevel(maxLogLevelFromEnv);
    if(level.has_value()){
      maxLogLevel = level.value();
    } else {
      std::cerr << "Warning: Unknown log level string found in environment: " <<
        MAX_LOG_LEVEL_ENV_VAR_NAME << " = " << maxLogLevelFromEnv << std::endl;
    }
  }
}

bool Logger::logThisLevel(const LogLevel &logLevel) {
  checkLogLevelInEnvironment();
  return (logLevel <= maxLogLevel);
}

void Logger::doLogging(const LogLevel &logLevel, const std::string &message) {
  std::unique_lock<std::shared_mutex> lock(mutex);
  if(logThisLevel(logLevel)){
    std::string timestamp = timeProvider->getTimeStampOfNow();
    for (const auto &currentLoggingDestination : loggingDestinations) {
      currentLoggingDestination->doLogging(logLevel, message, timestamp);
    }
  }
}

void Logger::removeAllDestinations() {
  std::unique_lock<std::shared_mutex> lock(mutex);
  loggingDestinations.clear();
}

std::size_t Logger::getCountOfLoggingDestinations() {
  std::shared_lock<std::shared_mutex> lock(mutex);
  return loggingDestinations.size();
}

void Logger::addLoggingDestination(std::unique_ptr<LoggingDestination> destination) {
  std::unique_lock<std::shared_mutex> lock(mutex);
  loggingDestinations.push_back(std::move(destination));
}

void Logger::setLoggingDestination(std::unique_ptr<LoggingDestination> destination) {
  removeAllDestinations();
  addLoggingDestination(std::move(destination));
}

void Logger::setMaxLogLevel(LogLevel logLevel) {
  std::unique_lock<std::shared_mutex> lock(mutex);
  maxLogLevel = logLevel;
}

}  // namespace second_take
