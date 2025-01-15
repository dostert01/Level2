#ifndef TIME_PROVIDERS_H
#define TIME_PROVIDERS_H

#include <string>

using namespace std;
namespace event_forge {

class TimeProvider {
 public:
  TimeProvider() = default;
  virtual string getTimeStampOfNow() = 0;
  virtual ~TimeProvider();
};

class DefaultTimeProvider : public TimeProvider {
 public:
  DefaultTimeProvider() = default;
  string getTimeStampOfNow() override;
};

}  // namespace event_forge

#endif  // TIME_PROVIDERS_H