#include <logger.h>

#include <iostream>
#include <memory>

namespace event_forge {

Logger::Logger() {
  logLevelStringMapper = LogLevelStringMapper();
  setMaxLogLevel(LogLevel::LOG_LEVEL_TRACE);
  timeProvider = make_unique<DefaultTimeProvider>();
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

void Logger::trace(const string &message) {
  doLogging(LogLevel::LOG_LEVEL_TRACE, message);
}

void Logger::debug(const string &message) {
  doLogging(LogLevel::LOG_LEVEL_DEBUG, message);
}

void Logger::info(const string &message) {
  doLogging(LogLevel::LOG_LEVEL_INFO, message);
}

void Logger::warn(const string &message) {
  doLogging(LogLevel::LOG_LEVEL_WARN, message);
}

void Logger::error(const string &message) {
  doLogging(LogLevel::LOG_LEVEL_ERROR, message);
}

void Logger::fatal(const string &message) {
  doLogging(LogLevel::LOG_LEVEL_FATAL, message);
}

void Logger::checkLogLevelInEnvironment() {
  const char* maxLogLevelFromEnv = getenv(MAX_LOG_LEVEL_ENV_VAR_NAME);
  if(maxLogLevelFromEnv != NULL) {
    optional<LogLevel> level = logLevelStringMapper.string2LogLevel(maxLogLevelFromEnv);
    if(level.has_value()){
      maxLogLevel = level.value();
    } else {
      cerr << "Warning: Unknown log level string found in environment: " <<
        MAX_LOG_LEVEL_ENV_VAR_NAME << " = " << maxLogLevelFromEnv << endl;
    }
  }
}

bool Logger::logThisLevel(const LogLevel &logLevel) {
  checkLogLevelInEnvironment();
  return (logLevel <= maxLogLevel);
}

void Logger::doLogging(const LogLevel &logLevel, const string &message) {
  unique_lock<shared_mutex> lock(mutex);
  if(logThisLevel(logLevel)){
    string timestamp = timeProvider->getTimeStampOfNow();
    for (const auto &currentLoggingDestination : loggingDestinations) {
      currentLoggingDestination->doLogging(logLevel, message, timestamp);
    }
  }
}

void Logger::removeAllDestinations() {
  unique_lock<shared_mutex> lock(mutex);
  loggingDestinations.clear();
}

size_t Logger::getCountOfLoggingDestinations() {
  shared_lock<shared_mutex> lock(mutex);
  return loggingDestinations.size();
}

void Logger::addLoggingDestination(unique_ptr<LoggingDestination> destination) {
  unique_lock<shared_mutex> lock(mutex);
  loggingDestinations.push_back(move(destination));
}

void Logger::setLoggingDestination(unique_ptr<LoggingDestination> destination) {
  removeAllDestinations();
  addLoggingDestination(move(destination));
}

void Logger::setMaxLogLevel(LogLevel logLevel) {
  unique_lock<shared_mutex> lock(mutex);
  maxLogLevel = logLevel;
}

}  // namespace event_forge
