#ifndef SECOND_TAKE_LOGGER
#define SECOND_TAKE_LOGGER

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <shared_mutex>

#include "loggingdestinations.h"
#include "loglevels.h"
#include "timeproviders.h"

#define LOGGER Logger::getInstance()

using namespace std;
namespace event_forge {

class Logger {
 public:
  virtual ~Logger();
  static Logger &getInstance();
  static Logger *getInstanceAsPointer();
  friend bool operator==(const Logger &lhs, const Logger &rhs);
  void trace(const string &message);
  void debug(const string &message);
  void info(const string &message);
  void warn(const string &message);
  void error(const string &message);
  void fatal(const string &message);
  void removeAllDestinations();
  size_t getCountOfLoggingDestinations();
  void addLoggingDestination(unique_ptr<LoggingDestination> destination);
  void setLoggingDestination(unique_ptr<LoggingDestination> destination);
  void setMaxLogLevel(LogLevel logLevel);

 private:
  Logger();
  unique_ptr<TimeProvider> timeProvider;
  vector<unique_ptr<LoggingDestination>> loggingDestinations;
  LogLevel maxLogLevel;
  LogLevelStringMapper logLevelStringMapper;
  shared_mutex mutex;

  void doLogging(const LogLevel &logLevel, const string &message);
  bool logThisLevel(const LogLevel &logLevel);
  void checkLogLevelInEnvironment();
};

}  // namespace event_forge

#endif  // SECOND_TAKE_LOGGER