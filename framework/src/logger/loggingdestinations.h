#ifndef LOGGING_DESTINATIONS_H
#define LOGGING_DESTINATIONS_H

#include <string>
#include <memory>

#include "loglevels.h"

namespace level2 {

#define SQUARE_BRACKET_LEFT "["
#define SQUARE_BRACKET_RIGHT "]"

//-------------------------------------------------------------------
class LoggingDestination {
 public:
  LoggingDestination() = default;
  virtual void doLogging(const LogLevel& logLevel, const std::string& message,
                         const std::string& timestamp) = 0;
  virtual ~LoggingDestination();
};

//-------------------------------------------------------------------
class LoggingDestinationStdOut : public LoggingDestination {
 public:
  LoggingDestinationStdOut() = default;
  void doLogging(const LogLevel& logLevel, const std::string& message,
                 const std::string& timestamp) override;
 protected:
  std::string prepareLoggingMessage(const LogLevel &logLevel,
                                    const std::string &message,
                                    const std::string &timestamp);
 private:
  LogLevelStringMapper loglevelMapper;
};

//-------------------------------------------------------------------
class LoggingDestinationSyslog : public LoggingDestination {
 public:
    LoggingDestinationSyslog(const std::string applicationName);
    ~LoggingDestinationSyslog();
    void doLogging(const LogLevel& logLevel, const std::string& message,
                 const std::string& timestamp) override;
  private:
    std::string syslogIdent;
};

//-------------------------------------------------------------------
class LoggingDestinationStdErr : public LoggingDestinationStdOut {
 public:
  void doLogging(const LogLevel& logLevel, const std::string& message,
                         const std::string& timestamp) override;
};

//-------------------------------------------------------------------
class LoggingDestinationFile : public LoggingDestinationStdOut {
 public:
  LoggingDestinationFile(const std::string fileName);
    void doLogging(const LogLevel& logLevel, const std::string& message,
                         const std::string& timestamp) override;
  private:
    std::string logFileName;
};

//-------------------------------------------------------------------
class LoggingDestinationFactory {
 public:
  LoggingDestinationFactory() = default;
  std::unique_ptr<LoggingDestination> createDestinationStdOut();
  std::unique_ptr<LoggingDestination> createDestinationStdErr();
  std::unique_ptr<LoggingDestination> createDestinationFile(const std::string fileName);
  std::unique_ptr<LoggingDestination> createDestinationSyslog(const std::string applicationName);
};

}  // namespace level2

#endif  // LOGGING_DESTINATIONS_H
