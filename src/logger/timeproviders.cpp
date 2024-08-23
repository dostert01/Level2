#include "timeproviders.h"

#include <ctime>

namespace second_take {

TimeProvider::~TimeProvider() {}

std::string DefaultTimeProvider::getTimeStampOfNow() {
  time_t timeBuffer;
  struct tm *localTime;
  char buffer[64];

  time(&timeBuffer);
  localTime = localtime(&timeBuffer);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
  std::string str(buffer);
  return str;
}

}  // namespace second_take