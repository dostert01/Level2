/**
 * @file logger.h
 * @brief Include this file if you want to use the LOGGER macro 
 */
#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <shared_mutex>

#include "loggingdestinations.h"
#include "loglevels.h"
#include "timeproviders.h"

/**
 * @def LOGGER
 * 
 * After including logger.h simply use this macro to gain access to
 * the logging functions of the singleton instance of the Logger
 */
#define LOGGER Logger::getInstance()

namespace level2 {

/**
 * @class Logger
 * @brief A configurable threat safe Logger
 * 
 * A threat safe singleton instance of class Logger can best be
 * used directly though the macro LOGGER. The instance is
 * automatically pre configured to use LogLevel::LOG_LEVEL_TRACE.
 * Additionally, the default logging destination is added to
 * output all logging to stdout. This default behaviour can be
 * changed by optionally removing the default logging destination,
 * adding one or more new ones and changing the maximum reported
 * loglevel to your likes
 * 
 * **Example:**
 * @code
 * // sets max log level to LogLevel::LOG_LEVEL_WARN
 * LOGGER.setMaxLogLevel(LogLevel::LOG_LEVEL_WARN);
 * // remove all pre configured logging destinations
 * LOGGER.removeAllDestinations();
 * // add two new logging destinations
 * LoggingDestinationFactory factory = LoggingDestinationFactory();
 * LOGGER.setLoggingDestination(factory.createDestinationSyslog("testapp"));
 * LOGGER.setLoggingDestination(factory.createDestinationStdErr());
 * @endcode
 */
class Logger {
 public:
  virtual ~Logger();
  /**
   * @brief One of the default factory methods
   * 
   * Instead of using this factory method you should prefer using the LOGGER
   * macro
   * 
   * @returns A reference to the one any only singleton Logger instance
   */
  static Logger &getInstance();
  /**
   * @brief One of the default factory methods
   * 
   * Instead of using this factory method you should prefer using the LOGGER
   * macro
   *
   * @returns A static pointer to the one any only singleton Logger instance
   */
  static Logger *getInstanceAsPointer();
  /**
   * Checks if two Loggers do actually refer to the same underlying pointer.
   * Because of the singleton nature of class Logger, this must always be the
   * case.
   * The operator is used in unit testing of the factory functions.
   */
  friend bool operator==(const Logger &lhs, const Logger &rhs);
  /**
   * @brief Logs a message using LogLevel::LOG_LEVEL_TRACE
   * @param message The message to be logged
   */
  void trace(const std::string &message);
  /**
   * @brief Logs a message using LogLevel::LOG_LEVEL_DEBUG
   * @param message The message to be logged
   */
  void debug(const std::string &message);
  /**
   * @brief Logs a message using LogLevel::LOG_LEVEL_INFO
   * @param message The message to be logged
   */
  void info(const std::string &message);
  /**
   * @brief Logs a message using LogLevel::LOG_LEVEL_WARN
   * @param message The message to be logged
   */
  void warn(const std::string &message);
  /**
   * @brief Logs a message using LogLevel::LOG_LEVEL_ERROR
   * @param message The message to be logged
   */
  void error(const std::string &message);
  /**
   * @brief Logs a message using LogLevel::LOG_LEVEL_FATAL
   * @param message The message to be logged
   */
  void fatal(const std::string &message);
  /**
   * @brief Removes all instances of LoggingDestination from Logger::loggingDestinations
   */
  void removeAllDestinations();
  /**
   * @brief Get back the count of added logging destinations.
   * @return the size_t of the internal instance
   * `std::vector<std::unique_ptr<LoggingDestination>> loggingDestinations`
   */
  size_t getCountOfLoggingDestinations();
  /**
   * @brief Add one more logging destinations.
   * 
   * **Example:**
   * @code
   * LoggingDestinationFactory factory = LoggingDestinationFactory();
   * LOGGER.addLoggingDestination(factory.createDestinationSyslog("testapp"));
   * @endcode
   */
  void addLoggingDestination(std::unique_ptr<LoggingDestination> destination);
  /**
   * @brief Replace all logging destinations by the new one.
   * 
   * **Example:**
   * @code
   * LoggingDestinationFactory factory = LoggingDestinationFactory();
   * LOGGER.setLoggingDestination(factory.createDestinationSyslog("testapp"));
   * @endcode
   */
  void setLoggingDestination(std::unique_ptr<LoggingDestination> destination);
  /**
   * Set the maximum log level to be reported
   */
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
