#pragma once

#include <string>
#include <memory>

#include "headerfieldowner.h"

namespace event_forge {


class HttpResponse : public HeaderFieldOwner {
    public:
        HttpResponse();
        ~HttpResponse();
        std::string getStatusCode();
        void setStatusCode(const std::string statusCodeString);
        std::string getProtocol();
        std::string getHeader();
        void setPayload(const std::string &payloadString);
        void setPayload(const void* payloadPointer, size_t lengthOfPayload);
        std::optional<char*> getPayloadPointer();
        size_t getContentLength();
    private:
        std::string statusCode;
        std::string protocol;
        char* payload;
};

} //namespace event_forge