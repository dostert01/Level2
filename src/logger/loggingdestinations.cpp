#include <iostream>
#include <fstream>

#include "loggingdestinations.h"

namespace second_take {

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
LoggingDestinationFile::LoggingDestinationFile(const std::string fileName){
    logFileName = fileName;
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

std::unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationFile(const std::string fileName) {
    std::unique_ptr<LoggingDestination> destination(
      new LoggingDestinationFile(fileName));
    return destination;
}

}  // namespace second_take
