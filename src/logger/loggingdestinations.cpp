#include <iostream>

#include "loggingdestinations.h"

namespace second_take {

std::unique_ptr<LoggingDestination> LoggingDestinationFactory::createDestinationStdOut() {
    std::unique_ptr<LoggingDestination> destination(
      new LoggingDestinationStdOut());
    return destination;
}

LoggingDestination::~LoggingDestination() {}

void LoggingDestinationStdOut::doLogging(const LogLevel &logLevel,
                                         const std::string &message,
                                         const std::string &timestamp) {
  std::cout << SQUARE_BRACKET_LEFT << loglevelMapper.logLevel2String(logLevel).value_or("")
            << SQUARE_BRACKET_RIGHT << " " << SQUARE_BRACKET_LEFT << timestamp
            << SQUARE_BRACKET_RIGHT << " " << message << std::endl;
}

}  // namespace second_take
