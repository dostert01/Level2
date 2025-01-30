#pragma once

#include <memory>
#include <map>
#include <optional>
#include <unistd.h>
#include <vector>

namespace event_forge {

class HttpException : public std::exception {
    public:
        explicit HttpException(const std::string& message);
        virtual const char* what() const noexcept override;
    private:
        std::string message;
}; 

using UrlParamsMultiMap = std::multimap<std::string, std::string>;
using UrlParamsRange = std::pair<UrlParamsMultiMap::const_iterator, UrlParamsMultiMap::const_iterator>;

class HttpRequest {
    public:
        HttpRequest();
        void setMethod(const std::string& method);
        std::string getMethod();
        void setPath(const std::string& pathValue);
        std::string getPath();
        std::optional<std::string> getHeaderFieldValue(std::string fieldName);
        void addHeaderField(std::string fieldName, std::string fieldValue);
        std::shared_ptr<std::map<std::string, std::string>> getAllHeaderFields();
        std::optional<int> getContentLength();
        int getCountOfUrlParams();
        UrlParamsRange getUrlParams(std::string searchKey);
        std::shared_ptr<UrlParamsMultiMap> getAllUrlParams();
        std::shared_ptr<std::vector<std::string>> getUrlParamValues(std::string searchKey);
        void setContentStartPointer(const char* p);
        std::optional<const char*> getContentPointer();
        bool hasPayload();
    private:
        std::string method;
        std::string path;
        const char* contentStart;
        std::shared_ptr<std::map<std::string, std::string>> headerFields;
        std::shared_ptr<UrlParamsMultiMap> urlParams;
        bool pathHasUrlParams();
        void parseUrlParamsFromPath();
        void addUrlParam(std::string key, std::string value);
};

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
        int contentStartOffset;
        bool readingFinished;
        std::shared_ptr<HttpRequest> request;
        void readFirst2K(int fileDescriptor);
        void readRemainingData(int fileDescriptor);
        void pollForMoreData(int fileDescriptor);
        void parseFirstLine(char* linebuffer);
        void parseHeaderFiledLine(char* linebuffer);
        void parseHeader();
};

} // namespace event_forge