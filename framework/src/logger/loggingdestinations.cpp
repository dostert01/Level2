#include <iostream>
#include <fstream>
#include <syslog.h>

#include "loggingdestinations.h"

namespace level2 {

//-------------------------------------------------------------------
LoggingDestination::~LoggingDestination() {}

//-------------------------------------------------------------------
void LoggingDestinationStdOut::doLogging(const LogLevel &logLevel,
                                         const std::string &message,
                                         const std::string &timestamp) {
  std::cout << prepareLoggingMessage(logLevel, message, timestamp) << std::endl;
}

std::string LoggingDestinationStdOut::prepareLoggingMessage(const LogLevel &logLevel,
                                    const std::string &message,
                                    const std::string &timestamp) {
    return SQUARE_BRACKET_LEFT + loglevelMapper.logLevel2String(logLevel).value_or("")
            + SQUARE_BRACKET_RIGHT + " " + SQUARE_BRACKET_LEFT + timestamp
            + SQUARE_BRACKET_RIGHT + " " + message;
}

//-------------------------------------------------------------------
void LoggingDestinationStdErr::doLogging(const LogLevel &logLevel,
                                         const std::string &message,
                                         const std::string &timestamp) {
  std::cerr << prepareLoggingMessage(logLevel, message, timestamp) << std::endl;
}

//-------------------------------------------------------------------
LoggingDestinationSyslog::LoggingDestinationSyslog(const std::string applicationName) {
    syslogIdent = applicationName;
    openlog(syslogIdent.c_str(), LOG_PID | LOG_CONS, LOG_USER);
}

LoggingDestinationSyslog::~LoggingDestinationSyslog() {
    closelog();
}

std::string LoggingDestinationSyslog::getSyslogIdent() {
    return syslogIdent;
}

void LoggingDestinationSyslog::doLogging(const LogLevel &logLevel,
                                         const std::string &message,
                                         const std::string &timestamp) {
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
LoggingDestinationFile::LoggingDestinationFile(const std::string fileName){
    logFileName = fileName;
}

std::string LoggingDestinationFile::getLogFileName() {
    return logFileName;
}

void LoggingDestinationFile::doLogging(const LogLevel& logLevel, const std::string& message,
                         const std::string& timestamp) {
    std::ofstream logfile(logFileName, std::ios::app | std::ios::out);
    if(logfile.is_open()) {
        logfile << prepareLoggingMessage(logLevel, message, timestamp) << std::endl;
        logfile.close();
    } else {
        std::cerr << "Failed to open logfile at: " << logFileName << std::endl;
    }
}
//-------------------------------------------------------------------
std::unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationStdOut() {
    std::unique_ptr<LoggingDestination> destination(
      new LoggingDestinationStdOut());
    return destination;
}

std::unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationStdErr() {
    std::unique_ptr<LoggingDestination> destination(
      new LoggingDestinationStdErr());
    return destination;
}

std::unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationSyslog(const std::string applicationName) {
    std::unique_ptr<LoggingDestination> destination(
      new LoggingDestinationSyslog(applicationName));
    return destination;
}

std::unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationFile(const std::string fileName) {
    std::unique_ptr<LoggingDestination> destination(
      new LoggingDestinationFile(fileName));
    return destination;
}

std::optional<std::unique_ptr<LoggingDestination>> LoggingDestinationFactory::createDestinationFromParamsMap(std::map<std::string, std::string>& params) {
    std::optional<std::unique_ptr<LoggingDestination>> returnValue;

    auto destinationType = params.find("type");
    if(destinationType != params.end()) {
        if(destinationType->second == LOGGING_DESTINATION_TYPE_STDOUT) {
            returnValue = createDestinationStdOut();
        } else if(destinationType->second == LOGGING_DESTINATION_TYPE_FILE) {
            auto fileName = params.find("fileName");
            if(fileName != params.end()) {
                returnValue = createDestinationFile(fileName->second);
            }
        } else if(destinationType->second == LOGGING_DESTINATION_TYPE_STDERR) {
            returnValue = createDestinationStdErr();
        } else if(destinationType->second == LOGGING_DESTINATION_TYPE_SYSLOG) {
            auto applicationName = params.find("applicationName");
            if(applicationName != params.end()) {
                returnValue = createDestinationSyslog(applicationName->second);
            }
        }
    }
    return returnValue;
}

}  // namespace level2
