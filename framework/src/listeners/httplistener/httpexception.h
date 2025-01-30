#pragma once

#include <string>

namespace event_forge {

class HttpException : public std::exception {
    public:
        explicit HttpException(const std::string& message);
        virtual const char* what() const noexcept override;
    private:
        std::string message;
}; 

}