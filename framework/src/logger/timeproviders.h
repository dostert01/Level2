#pragma once

#include <string>

namespace event_forge {

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

}  // namespace event_forge
