#pragma once

#include <string>

namespace event_forge {

class HttpException : public std::exception {
    public:
        HttpException(const HttpException &other);
        explicit HttpException(const std::string& message);
        HttpException(const std::string& message, const std::string& httpReturnCode);
        virtual const char* what() const noexcept override;
        bool hastHttpReturnCode();
        std::string getHttpReturnCode();
    private:
        std::string message;
        std::string suggestedHttpReturnCode;
}; 

}