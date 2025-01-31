#include <string.h>
#include <cstring>
#include <poll.h>

#include "http11.h"
#include "logger.h"
#include "httpdefs.h"

namespace event_forge {

Http11::Http11(int fileDescriptor) {
    rawDataBuffer = NULL;
    lineBuffer = NULL;
    clientSocketFileDescriptor = fileDescriptor;
}

Http11::~Http11() {
    if(rawDataBuffer) {
        free(rawDataBuffer);
        rawDataBuffer = NULL;
    }
    if(lineBuffer) {
        free(lineBuffer);
        lineBuffer = NULL;
    }
}

void Http11::sendResponse(HttpResponse &response) {
  void *message;
  size_t messageSize = response.getMessagePointer(message);
  write(clientSocketFileDescriptor, message, messageSize);
}

std::optional<std::shared_ptr<HttpRequest>> Http11::readRequest(std::string clientHostName) {
  std::optional<std::shared_ptr<HttpRequest>> returnValue = std::nullopt;
  try {
    readFirst2K();
    parseHeader();
    readRemainingData();
    returnValue = request;
  } catch (const std::exception& e) {
    LOGGER.error("Failed reading incoming request: " + std::string(e.what()));
    if(HttpException *ex = dynamic_cast<HttpException*>((std::exception*)&e); ex != nullptr) {
      LOGGER.trace("Exception is of class HttpException. Http statuscode: '" + ex->getHttpReturnCode() + "'");
    }
  }
  return returnValue;
}

ssize_t Http11::getBytesRead() {
    return bytesRead;
}

void Http11::parseHeader() {
  char* headerEnd = std::strstr(rawDataBuffer, HEADER_TERMINATOR);
  if(!headerEnd) {
    throw HttpException("first " + std::to_string(bytesRead) +
      " bytes of message do not contain a valid header!", HTTP_STATUS_CODE_431);
  }
  contentStartOffset = (headerEnd - rawDataBuffer) + strlen(HEADER_TERMINATOR);
  char *currentLineStart = rawDataBuffer;
  char *currentLineEnd = NULL;
  request = std::make_shared<HttpRequest>();
  while(currentLineStart < headerEnd) {
    currentLineEnd = std::strstr(currentLineStart, LINE_TERMINATOR);
    if(currentLineEnd) {
      parseSingleHeaderLine(currentLineEnd, currentLineStart);
    }
  }
}

void Http11::parseSingleHeaderLine(char* currentLineEnd, char*& currentLineStart) {
  int bufferSize = (currentLineEnd - currentLineStart) + 1;
  lineBuffer = (char*)realloc(lineBuffer, bufferSize);
  memset(lineBuffer, 0, bufferSize);
  memcpy(lineBuffer, currentLineStart, bufferSize - 1);
  if (currentLineStart == rawDataBuffer) {
    parseFirstLine(lineBuffer);
  } else {
    parseHeaderFieldLine(lineBuffer);
  }
  currentLineStart = currentLineEnd + strlen(LINE_TERMINATOR);
}

void Http11::parseFirstLine(char* linebuffer) {
    char* lineEnd = linebuffer + strlen(linebuffer);
    char* methodEnd = std::strstr(linebuffer, METHOD_PATH_SEPARATOR);
    if(!methodEnd) {
        throw HttpException("no http method found in header!", HTTP_STATUS_CODE_400);
    }
    request->setMethod(std::string(linebuffer, methodEnd - linebuffer));
    char* pathStart = methodEnd + 1;
    if(pathStart < lineEnd) {
        char* pathEnd = std::strstr(linebuffer, PATH_PROTOCOL_SEPARATOR);
        if(!pathEnd) {
            throw HttpException("no path found in http header!", HTTP_STATUS_CODE_400);
        }
        request->setPath(std::string(pathStart, pathEnd - pathStart));
    }
}

void Http11::parseHeaderFieldLine(char* linebuffer) {
    char* lineEnd = linebuffer + strlen(linebuffer);
    char* fieldNameEnd = std::strstr(linebuffer, HEADER_FIELD_SEPARATOR);
    if(!fieldNameEnd) {
        throw HttpException("no http header field found in '" + std::string(linebuffer) + "'", HTTP_STATUS_CODE_400);
    }
    std::string fieldName(linebuffer, fieldNameEnd - linebuffer);
    char* fieldValueStart = fieldNameEnd + strlen(HEADER_FIELD_SEPARATOR);
    if(fieldValueStart > lineEnd) {
        throw HttpException("http header field '" + fieldName + "' has no value!", HTTP_STATUS_CODE_400);
    }
    std::string fieldValue(fieldValueStart);
    request->addHeaderField(fieldName, fieldValue);
}

void Http11::readFirst2K() {
  if(clientSocketFileDescriptor < 0) {
    throw HttpException("readFirst2K called with invalid file descriptor: '"
      + std::string(strerror(errno)), HTTP_STATUS_CODE_500);
  }
  char buffer[READ_BUFFER_SIZE];
  memset(buffer, 0, READ_BUFFER_SIZE);
  bytesRead = 0;
  rawDataBuffer = NULL;
  ssize_t chunkSize;
  while ((bytesRead < MAX_READ) && ((chunkSize = read(clientSocketFileDescriptor, buffer, MAX_READ)) > 0)) {
    if (chunkSize > 0) {
      rawDataBuffer = (char*)realloc(rawDataBuffer, bytesRead + chunkSize + 1);
      memcpy(rawDataBuffer + bytesRead, buffer, chunkSize);
      bytesRead += chunkSize;
      rawDataBuffer[bytesRead] = 0;
    } else if (chunkSize < 0) {
        pollForMoreData();
    }
  }
  readingFinished = (chunkSize == 0);
}

void Http11::readRemainingData() {
  if(!readingFinished && request->getContentLength().has_value()) {
      int expectedMessageLength = request->getContentLength().value() + contentStartOffset;
      LOGGER.trace("continuing to read from socket - " + std::to_string(bytesRead) + " of " + std::to_string(expectedMessageLength) + " byted have been read. Trying to fetch the rest ...");
      char buffer[READ_BUFFER_SIZE];
      memset(buffer, 0, READ_BUFFER_SIZE);
      ssize_t chunkSize;
      while ((bytesRead < expectedMessageLength) && ((chunkSize = read(clientSocketFileDescriptor, buffer, MAX_READ)) > 0)) {
          if (chunkSize > 0) {
              rawDataBuffer = (char*)realloc(rawDataBuffer, bytesRead + chunkSize + 1);
              memcpy(rawDataBuffer + bytesRead, buffer, chunkSize);
              bytesRead += chunkSize;
              rawDataBuffer[bytesRead] = 0;
          } else if (chunkSize < 0) {
            pollForMoreData();
          } else {
              throw HttpException("error during reading from socket. Maybe client has close the connection: '" +
                std::string(strerror(errno)), HTTP_STATUS_CODE_400);
          }
      }
      readingFinished = (chunkSize == 0);
      contentStart = rawDataBuffer + contentStartOffset;
      request->setContentStartPointer(contentStart);
      LOGGER.trace("finished reading from socket - " + std::to_string(bytesRead) + " of " + std::to_string(expectedMessageLength) + " byted have been read.");
  } else if(!readingFinished && !request->getContentLength().has_value()) {
      throw HttpException("Content-Length required", HTTP_STATUS_CODE_411);
  }
}

void Http11::pollForMoreData() {
  if ((errno == EAGAIN) && (errno == EWOULDBLOCK)) {
    struct pollfd fds = {clientSocketFileDescriptor, POLLIN, 0};
    int failCounter = 0;
    while ((failCounter++ < TIMEOUT_IN_SECONDS) && (poll(&fds, 1, ONE_SECOND) == 0));
    if(failCounter >= TIMEOUT_IN_SECONDS) {
        throw HttpException("connection to client timed out while waiting for more data to arrive: '" +
                        std::string(strerror(errno)), HTTP_STATUS_CODE_408);
    }
  } else {
    throw HttpException("error during reading from socket file descriptor: '" +
                        std::string(strerror(errno)), HTTP_STATUS_CODE_400);
  }
}


} // namespace event_forge

