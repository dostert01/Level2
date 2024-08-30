#ifndef TIME_PROVIDERS_H
#define TIME_PROVIDERS_H

#include <string>

namespace second_take {

class TimeProvider {
 public:
  TimeProvider() = default;
  virtual std::string getTimeStampOfNow() = 0;
  virtual ~TimeProvider();
};

class DefaultTimeProvider : public TimeProvider {
 public:
  DefaultTimeProvider() = default;
  std::string getTimeStampOfNow() override;
};

}  // namespace second_take

#endif  // TIME_PROVIDERS_H