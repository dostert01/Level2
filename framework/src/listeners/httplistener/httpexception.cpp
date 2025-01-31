#include "httpexception.h"

namespace event_forge {

HttpException::HttpException(const std::string& message) : exception() {
    this->message = message;
    suggestedHttpReturnCode = "";
}

HttpException::HttpException(const std::string& message, const std::string& httpReturnCode) : HttpException(message) {
    suggestedHttpReturnCode = httpReturnCode;
}

const char* HttpException::what() const noexcept {
    return (char*)message.c_str();
}

bool HttpException::hastHttpReturnCode() {
    return !suggestedHttpReturnCode.empty();
}

std::string HttpException::getHttpReturnCode() {
    return suggestedHttpReturnCode;
}


} //namespace event_forge