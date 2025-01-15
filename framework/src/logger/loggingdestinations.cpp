#include <iostream>
#include <fstream>
#include <syslog.h>

#include "loggingdestinations.h"

namespace event_forge {

//-------------------------------------------------------------------
LoggingDestination::~LoggingDestination() {}

//-------------------------------------------------------------------
void LoggingDestinationStdOut::doLogging(const LogLevel &logLevel,
                                         const string &message,
                                         const string &timestamp) {
  cout << prepareLoggingMessage(logLevel, message, timestamp) << endl;
}

string LoggingDestinationStdOut::prepareLoggingMessage(const LogLevel &logLevel,
                                    const string &message,
                                    const string &timestamp) {
    return SQUARE_BRACKET_LEFT + loglevelMapper.logLevel2String(logLevel).value_or("")
            + SQUARE_BRACKET_RIGHT + " " + SQUARE_BRACKET_LEFT + timestamp
            + SQUARE_BRACKET_RIGHT + " " + message;
}

//-------------------------------------------------------------------
void LoggingDestinationStdErr::doLogging(const LogLevel &logLevel,
                                         const string &message,
                                         const string &timestamp) {
  cerr << prepareLoggingMessage(logLevel, message, timestamp) << endl;
}

//-------------------------------------------------------------------
LoggingDestinationSyslog::LoggingDestinationSyslog(const string applicationName) {
    syslogIdent = applicationName;
    openlog(syslogIdent.c_str(), LOG_PID | LOG_CONS, LOG_USER);
}

LoggingDestinationSyslog::~LoggingDestinationSyslog() {
    closelog();
}

void LoggingDestinationSyslog::doLogging(const LogLevel &logLevel,
                                         const string &message,
                                         const string &timestamp) {
    if(logLevel == LogLevel::LOG_LEVEL_DEBUG)
        syslog(LOG_DEBUG, "%s", message.c_str());
    else if(logLevel == LogLevel::LOG_LEVEL_ERROR)
        syslog(LOG_ERR, "%s", message.c_str());
    else if(logLevel == LogLevel::LOG_LEVEL_FATAL)
        syslog(LOG_CRIT, "%s", message.c_str());
    else if(logLevel == LogLevel::LOG_LEVEL_INFO)
        syslog(LOG_INFO, "%s", message.c_str());
    else if(logLevel == LogLevel::LOG_LEVEL_TRACE)
        syslog(LOG_DEBUG, "%s", message.c_str());
    else if(logLevel == LogLevel::LOG_LEVEL_WARN)
        syslog(LOG_WARNING, "%s", message.c_str());
}

//-------------------------------------------------------------------
LoggingDestinationFile::LoggingDestinationFile(const string fileName){
    logFileName = fileName;
}

void LoggingDestinationFile::doLogging(const LogLevel& logLevel, const string& message,
                         const string& timestamp) {
    ofstream logfile(logFileName, ios::app | ios::out);
    if(logfile.is_open()) {
        logfile << prepareLoggingMessage(logLevel, message, timestamp) << endl;
        logfile.close();
    } else {
        cerr << "Failed to open logfile at: " << logFileName << endl;
    }
}
//-------------------------------------------------------------------
unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationStdOut() {
    unique_ptr<LoggingDestination> destination(
      new LoggingDestinationStdOut());
    return destination;
}

unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationStdErr() {
    unique_ptr<LoggingDestination> destination(
      new LoggingDestinationStdErr());
    return destination;
}

unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationSyslog(const string applicationName) {
    unique_ptr<LoggingDestination> destination(
      new LoggingDestinationSyslog(applicationName));
    return destination;
}

unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationFile(const string fileName) {
    unique_ptr<LoggingDestination> destination(
      new LoggingDestinationFile(fileName));
    return destination;
}

}  // namespace event_forge
