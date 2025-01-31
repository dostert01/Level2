#pragma once

#include <memory>
#include <map>
#include <optional>
#include <unistd.h>
#include <vector>

#include "httpexception.h"
#include "httprequest.h"
#include "httpresponse.h"

namespace event_forge {


class Http11 {
    public:
        Http11();
        ~Http11();
        std::optional<std::shared_ptr<HttpRequest>> readRequest(int fileDescriptor, std::string clientHostName);
        ssize_t getBytesRead();
       private:
        ssize_t bytesRead;
        char* rawDataBuffer;
        char* contentStart;
        char* lineBuffer;
        int contentStartOffset;
        bool readingFinished;
        std::shared_ptr<HttpRequest> request;
        void readFirst2K(int fileDescriptor);
        void readRemainingData(int fileDescriptor);
        void pollForMoreData(int fileDescriptor);
        void parseFirstLine(char* linebuffer);
        void parseHeaderFieldLine(char* linebuffer);
        void parseHeader();
        void parseSingleHeaderLine(char* currentLineEnd, char*& currentLineStart);
};

} // namespace event_forge