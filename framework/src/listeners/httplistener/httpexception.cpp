#include "httpexception.h"

namespace level2 {

HttpException::HttpException(const HttpException &other) : exception() {
    this->message = other.message;
    this->suggestedHttpReturnCode = other.suggestedHttpReturnCode;
}

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


} //namespace level2