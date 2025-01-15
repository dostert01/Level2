#ifndef LOGGING_DESTINATIONS_H
#define LOGGING_DESTINATIONS_H

#include <string>
#include <memory>

#include "loglevels.h"

using namespace std;
namespace event_forge {

#define SQUARE_BRACKET_LEFT "["
#define SQUARE_BRACKET_RIGHT "]"

//-------------------------------------------------------------------
class LoggingDestination {
 public:
  LoggingDestination() = default;
  virtual void doLogging(const LogLevel& logLevel, const string& message,
                         const string& timestamp) = 0;
  virtual ~LoggingDestination();
};

//-------------------------------------------------------------------
class LoggingDestinationStdOut : public LoggingDestination {
 public:
  LoggingDestinationStdOut() = default;
  void doLogging(const LogLevel& logLevel, const string& message,
                 const string& timestamp) override;
 protected:
  string prepareLoggingMessage(const LogLevel &logLevel,
                                    const string &message,
                                    const string &timestamp);
 private:
  LogLevelStringMapper loglevelMapper;
};

//-------------------------------------------------------------------
class LoggingDestinationSyslog : public LoggingDestination {
 public:
    LoggingDestinationSyslog(const string applicationName);
    ~LoggingDestinationSyslog();
    void doLogging(const LogLevel& logLevel, const string& message,
                 const string& timestamp) override;
  private:
    string syslogIdent;
};

//-------------------------------------------------------------------
class LoggingDestinationStdErr : public LoggingDestinationStdOut {
 public:
  void doLogging(const LogLevel& logLevel, const string& message,
                         const string& timestamp) override;
};

//-------------------------------------------------------------------
class LoggingDestinationFile : public LoggingDestinationStdOut {
 public:
  LoggingDestinationFile(const string fileName);
    void doLogging(const LogLevel& logLevel, const string& message,
                         const string& timestamp) override;
  private:
    string logFileName;
};

//-------------------------------------------------------------------
class LoggingDestinationFactory {
 public:
  LoggingDestinationFactory() = default;
  unique_ptr<LoggingDestination> createDestinationStdOut();
  unique_ptr<LoggingDestination> createDestinationStdErr();
  unique_ptr<LoggingDestination> createDestinationFile(const string fileName);
  unique_ptr<LoggingDestination> createDestinationSyslog(const string applicationName);
};

}  // namespace event_forge

#endif  // LOGGING_DESTINATIONS_H
