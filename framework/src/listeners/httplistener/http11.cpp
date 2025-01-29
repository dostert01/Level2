#include <string.h>
#include <cstring>
#include <sstream>

#include "http11.h"
#include "logger.h"

#define READ_BUFFER_SIZE 2049
#define MAX_READ (READ_BUFFER_SIZE - 1)
#define LINE_TERMINATOR "\r\n"
#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_FIELD_SEPARATOR ": "
#define METHOD_PATH_SEPARATOR " /"
#define PATH_PROTOCOL_SEPARATOR " HTTP/1.1"
#define PATH_QUERY_SEPARATOR "?"
#define QUERY_PARAM_SEPARATOR '&'
#define KEY_VALUE_SEPARATOR "="

namespace event_forge {

//-------------------------------------------------------------------
HttpRequest::HttpRequest() {
    urlParams = std::make_shared<UrlParamsMultiMap>();
}

void HttpRequest::setMethod(const std::string& method) {
    this->method = method;
}

std::string HttpRequest::getMethod() {
    return method;   
}

void HttpRequest::setPath(const std::string& pathValue) {
    path = pathValue;
    if(pathHasUrlParams()) {
        parseUrlParamsFromPath();
    }
}

std::string HttpRequest::getPath() {
    return path;
}

std::optional<std::string> HttpRequest::getHeaderFieldValue(std::string fieldName) {
    std::optional<std::string> returnValue = std::nullopt;
    auto found = headerFields.find(fieldName);
    if(found != headerFields.end()) {
        returnValue = found->second;
    }
    return returnValue;
}

void HttpRequest::addHeaderField(std::string fieldName, std::string fieldValue) {
    LOGGER.trace("HttpRequest - adding header field: " + fieldName + ": " + fieldValue);
    headerFields[fieldName] = fieldValue;
}

int HttpRequest::getCountOfUrlParams() {
    return urlParams->size();
}

bool HttpRequest::pathHasUrlParams() {
    return path.find(PATH_QUERY_SEPARATOR) != std::string::npos;
}

void HttpRequest::parseUrlParamsFromPath() {
    int queryStringStart = path.find(PATH_QUERY_SEPARATOR);
    if(queryStringStart != std::string::npos) {
        std::istringstream queryString(path.substr(queryStringStart + 1));
        path = path.substr(0, queryStringStart);
        std::string singleParam;
        while(std::getline(queryString, singleParam, QUERY_PARAM_SEPARATOR)) {
            int pos;
            if((pos = singleParam.find(KEY_VALUE_SEPARATOR)) != std::string::npos) {
                addUrlParam(singleParam.substr(0, pos), singleParam.substr(pos + 1));
            }
        }
    }
}

void HttpRequest::addUrlParam(std::string key, std::string value) {
    urlParams->insert({key, value});
}

UrlParamsRange HttpRequest::getUrlParams(std::string searchKey) {
    auto returnValue = urlParams->equal_range(searchKey);
    return returnValue;
}

std::shared_ptr<UrlParamsMultiMap>  HttpRequest::getAllUrlParams() {
    return urlParams;
}

//-------------------------------------------------------------------

Http11::~Http11() {
    if(rawDataBuffer) {
        free(rawDataBuffer);
        rawDataBuffer = NULL;
    }
}

std::optional<std::shared_ptr<HttpRequest>> Http11::readRequest(int fileDescriptor, std::string clientHostName) {
  std::optional<std::shared_ptr<HttpRequest>> returnValue = std::nullopt;
  try {
    readFirst2K(fileDescriptor);
    parseHeader();
    returnValue = request;
  } catch (const std::exception& e) {
    LOGGER.error("Failed reading incoming request: " + std::string(e.what()));
  }
  return returnValue;
}

void Http11::parseHeader() {
  char* headerEnd = std::strstr(rawDataBuffer, HEADER_TERMINATOR);
  if(!headerEnd) {
    throw HttpException("first " + std::to_string(bytesRead) + " bytes of message do not contain a valid header!");
  }
  char *currentLineStart = rawDataBuffer;
  char *currentLineEnd = NULL;
  char *lineBuffer = NULL;
  request = std::make_shared<HttpRequest>();
  while(currentLineStart < headerEnd) {
    currentLineEnd = std::strstr(currentLineStart, LINE_TERMINATOR);
    if(currentLineEnd) {
        int bufferSize = (currentLineEnd - currentLineStart) + 1;
        lineBuffer = (char*)realloc(lineBuffer, bufferSize);
        memset(lineBuffer, 0, bufferSize);
        memcpy(lineBuffer, currentLineStart, bufferSize - 1);
        if(currentLineStart == rawDataBuffer) {
            parseFirstLine(lineBuffer);
        } else {
            parseHeaderFiledLine(lineBuffer);
        }
        currentLineStart = currentLineEnd + strlen(LINE_TERMINATOR);
    }
  }
  if(lineBuffer) {
    free(lineBuffer);
    lineBuffer = NULL;
  }    
}

void Http11::parseFirstLine(char* linebuffer) {
    char* lineEnd = linebuffer + strlen(linebuffer);
    char* methodEnd = std::strstr(linebuffer, METHOD_PATH_SEPARATOR);
    if(!methodEnd) {
        throw HttpException("no http method found in header!");
    }
    request->setMethod(std::string(linebuffer, methodEnd - linebuffer));
    char* pathStart = methodEnd + 1;
    if(pathStart < lineEnd) {
        char* pathEnd = std::strstr(linebuffer, PATH_PROTOCOL_SEPARATOR);
        if(!pathEnd) {
            throw HttpException("no path found in http header!");
        }
        request->setPath(std::string(pathStart, pathEnd - pathStart));
    }
}

void Http11::parseHeaderFiledLine(char* linebuffer) {
    char* lineEnd = linebuffer + strlen(linebuffer);
    char* fieldNameEnd = std::strstr(linebuffer, HEADER_FIELD_SEPARATOR);
    if(!fieldNameEnd) {
        throw HttpException("no http header field found in '" + std::string(linebuffer) + "'");
    }
    std::string fieldName(linebuffer, fieldNameEnd - linebuffer);
    char* fieldValueStart = fieldNameEnd + strlen(HEADER_FIELD_SEPARATOR);
    if(fieldValueStart > lineEnd) {
        throw HttpException("http header field '" + fieldName + "' has no value!");
    }
    std::string fieldValue(fieldValueStart);
    request->addHeaderField(fieldName, fieldValue);
}

void Http11::readFirst2K(int fileDescriptor) {
  char buffer[READ_BUFFER_SIZE];
  memset(buffer, 0, READ_BUFFER_SIZE);
  bytesRead = 0;
  ssize_t chunkSize;
  rawDataBuffer = NULL;
  while (((chunkSize = read(fileDescriptor, buffer, MAX_READ)) > 0) && (bytesRead < MAX_READ)) {
    if (chunkSize > 0) {
      rawDataBuffer = (char*)realloc(rawDataBuffer, bytesRead + chunkSize + 1);
      memcpy(rawDataBuffer + bytesRead, buffer, chunkSize);
      bytesRead += chunkSize;
      rawDataBuffer[bytesRead] = 0;
    } else if (chunkSize < 0) {
        throw HttpException("error during reading from socket file descriptor: '" + std::string(strerror(errno)));
    }
  }
  readingFinished = (chunkSize == 0);
}

//-------------------------------------------------------------------
HttpException::HttpException(const std::string& message) : exception() {
    this->message = message;
}

const char* HttpException::what() const noexcept {
    return (char*)message.c_str();
}


} // namespace event_forge

