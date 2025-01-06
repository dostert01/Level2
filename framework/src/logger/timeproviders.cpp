#include "timeproviders.h"

#include <ctime>

namespace event_forge {

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

}  // namespace event_forge