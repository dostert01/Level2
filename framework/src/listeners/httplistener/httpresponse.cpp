#include <stdlib.h>
#include <string.h>
#include <sstream>

#include "httpresponse.h"
#include "httpdefs.h"
#include "logger.h"

#define BLANK " "

namespace level2 {


HttpResponse::HttpResponse() : HeaderFieldOwner() {
    messagePointer = NULL;
    payload = NULL;
    setStatusCode(HTTP_STATUS_CODE_200);
    protocol = HTTP_PROTOCOL_VERSION;
    addHeaderField(HTTP_FIELD_NAME_CONNECTION, HTTP_FIELD_VALUE_CLOSE);
    addHeaderField(HTTP_FIELD_NAME_CONTENT_TYPE, PAYLOAD_MIMETYPE_TEXT_PLAIN_UTF8);
    addHeaderField(HTTP_FIELD_NAME_SERVER, HTTP_FIELD_VALUE_SERVER);
}

HttpResponse::~HttpResponse() {
    if(payload) {
        free(payload);
        payload = NULL;
    }
    if(messagePointer) {
        free(messagePointer);
        messagePointer = NULL;
    }
}

void HttpResponse::setStatusCode(const std::string statusCodeString) {
    statusCode = statusCodeString;
    if((!payload) || !statusCodeString.starts_with("2")) {
        setPayload(statusCodeString);
    }
}

std::string HttpResponse::getStatusCode() {
    return statusCode;
}

std::string HttpResponse::getProtocol() {
    return protocol;
}

std::string HttpResponse::getHeader() {
    std::string headerAsString(HTTP_PROTOCOL_VERSION);
    headerAsString.append(BLANK).append(statusCode).append(LINE_TERMINATOR);
    auto headerFields = getAllHeaderFields();
    for(auto headerField : *headerFields) {
        headerAsString.append(headerField.first).append(HEADER_FIELD_SEPARATOR).append(headerField.second).append(LINE_TERMINATOR);
    }
    headerAsString.append(HEADER_TERMINATOR);
    return headerAsString;
}

void HttpResponse::setPayload(const std::string &payloadString) {
    setPayload(payloadString.c_str(), strlen(payloadString.c_str()));
}

void HttpResponse::setPayload(const void* payloadPointer, size_t lengthOfPayload) {
    size_t payloadBufferSize = lengthOfPayload + 1;
    payload = (char*)realloc(payload, payloadBufferSize);
    memset(payload, 0, payloadBufferSize);
    memcpy(payload, payloadPointer, payloadBufferSize - 1);
    addHeaderField(HTTP_FIELD_NAME_CONTENT_LENGTH, std::to_string(lengthOfPayload));
}

std::optional<char*> HttpResponse::getPayloadPointer() {
    std::optional<char*> returnValue = std::nullopt;
    if(payload) {
        returnValue = payload;
    }
    return returnValue;
}

size_t HttpResponse::getMessagePointer(void* &message) {
    std::string header = getHeader();
    size_t headerLen = strlen(header.c_str());
    size_t contentLength = getContentLength();
    size_t messageLen = contentLength + headerLen;
    this->messagePointer = (char*)realloc(this->messagePointer, messageLen);
    memcpy(this->messagePointer, header.c_str(), headerLen);
    auto messageBody = getPayloadPointer();
    if((contentLength > 0) && (messageBody.has_value())) {
        memcpy(this->messagePointer + headerLen, messageBody.value(), contentLength);
    }
    message = this->messagePointer;
    return messageLen;
}

size_t HttpResponse::getContentLength() {
    std::string contentLengthString = getHeaderFieldValue(HTTP_FIELD_NAME_CONTENT_LENGTH).value_or("0");
    size_t contentLength;
    std::istringstream toConvert(contentLengthString);
    toConvert >> contentLength;
    if(toConvert.fail()) {
        LOGGER.error("failed converting Content-Length value '" + contentLengthString + "' to numeric. Assuming Conent-Length is 0");
        contentLength = 0;
    }
    return contentLength;
}

}