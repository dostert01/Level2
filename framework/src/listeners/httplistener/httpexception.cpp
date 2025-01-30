#include "httpexception.h"

namespace event_forge {

HttpException::HttpException(const std::string& message) : exception() {
    this->message = message;
}

const char* HttpException::what() const noexcept {
    return (char*)message.c_str();
}

} //namespace event_forge