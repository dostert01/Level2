#include <sstream>
#include "httprequest.h"
#include "logger.h"
#include "httpdefs.h"

namespace event_forge {

HttpRequest::HttpRequest() {
    urlParams = std::make_shared<UrlParamsMultiMap>();
    headerFields = std::make_shared<std::map<std::string, std::string>>();
    contentStart = NULL;
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

void HttpRequest::setContentStartPointer(const char* p) {
    contentStart = p;
}

std::optional<const char*> HttpRequest::getContentPointer() {
    std::optional<const char*> returnValue = std::nullopt;
    if(contentStart != NULL) {
        LOGGER.trace("returning valid content pointer");
        returnValue = contentStart;
    } else {
        LOGGER.trace("returning empty content pointer");
    }
    return returnValue;
}

bool HttpRequest::hasPayload() {
    return getContentPointer().has_value();
}

std::optional<std::string> HttpRequest::getHeaderFieldValue(std::string fieldName) {
    std::optional<std::string> returnValue = std::nullopt;
    auto found = headerFields->find(fieldName);
    if(found != headerFields->end()) {
        returnValue = found->second;
    }
    return returnValue;
}

void HttpRequest::addHeaderField(std::string fieldName, std::string fieldValue) {
    LOGGER.trace("HttpRequest - adding header field: " + fieldName + ": " + fieldValue);
    headerFields->insert({fieldName, fieldValue});
}

std::shared_ptr<std::map<std::string, std::string>> HttpRequest::getAllHeaderFields() {
    return headerFields;
}

std::optional<int> HttpRequest::getContentLength() {
    std::optional<int> returnValue = std::nullopt;
    auto contentLengthString = getHeaderFieldValue("Content-Length");
    if(contentLengthString.has_value()) {
        try {
            returnValue = std::stoi(contentLengthString.value());
        } catch (std::exception const& e) {
            LOGGER.error("failed to read Content-Length. Value '" + contentLengthString.value() + "' is invalid. Assuming no Content-Length has been sent by the client.");
            returnValue = std::nullopt;
        }
    }
    return returnValue;
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

std::shared_ptr<UrlParamsMultiMap> HttpRequest::getAllUrlParams() {
    return urlParams;
}

std::shared_ptr<std::vector<std::string>> HttpRequest::getUrlParamValues(std::string searchKey) {
    auto returnValue = std::make_shared<std::vector<std::string>>();
    auto params = getUrlParams(searchKey);
    
    for(auto param = params.first; param != params.second; ++param) {
        returnValue->push_back(param->second);
    }
    return returnValue;
}

}