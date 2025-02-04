#pragma once

#include <memory>
#include <map>
#include <optional>
#include <unistd.h>
#include <vector>

#include "httpexception.h"
#include "httprequest.h"
#include "httpresponse.h"

namespace level2 {


class Http11 {
    public:
        Http11(int fileDescriptor);
        ~Http11();
        std::optional<std::shared_ptr<HttpRequest>> readRequest(std::string clientHostName);
        ssize_t getBytesRead();
        void sendResponse(HttpResponse &response);
        bool hasErrors();
        std::optional<HttpException> getLastError();
       private:
        ssize_t bytesRead;
        char* rawDataBuffer;
        char* contentStart;
        char* lineBuffer;
        int contentStartOffset;
        bool readingFinished;
        int clientSocketFileDescriptor;
        std::shared_ptr<HttpRequest> request;
        std::shared_ptr<std::vector<HttpException>> httpExceptions;
        void readFirst2K();
        void readRemainingData();
        void pollForMoreData();
        void parseFirstLine(char* linebuffer);
        void parseHeaderFieldLine(char* linebuffer);
        void parseHeader();
        void parseSingleHeaderLine(char* currentLineEnd, char*& currentLineStart);
};

} // namespace level2