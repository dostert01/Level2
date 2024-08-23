#include "loggingdestinations.h"

#include <iostream>

namespace second_take {

LoggingDestination::~LoggingDestination() {}

void LoggingDestinationStdOut::doLogging(const LogLevel &logLevel,
                                         const std::string &message,
                                         const std::string &timestamp) {
  std::cout << SQUARE_BRACKET_LEFT << loglevelMapper.logLevel2String(logLevel).value_or("")
            << SQUARE_BRACKET_RIGHT << " " << SQUARE_BRACKET_LEFT << timestamp
            << SQUARE_BRACKET_RIGHT << " " << message << std::endl;
}

}  // namespace second_take
