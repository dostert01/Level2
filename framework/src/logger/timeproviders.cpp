#include "timeproviders.h"

#include <ctime>

namespace event_forge {

TimeProvider::~TimeProvider() {}

string DefaultTimeProvider::getTimeStampOfNow() {
  time_t timeBuffer;
  struct tm *localTime;
  char buffer[64];

  time(&timeBuffer);
  localTime = localtime(&timeBuffer);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
  string str(buffer);
  return str;
}

}  // namespace event_forge