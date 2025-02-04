#include "headerfieldowner.h"
#include "logger.h"

namespace level2 {

HeaderFieldOwner::HeaderFieldOwner() {
    headerFields = std::make_shared<std::map<std::string, std::string>>();
}

std::optional<std::string> HeaderFieldOwner::getHeaderFieldValue(std::string fieldName) {
    std::optional<std::string> returnValue = std::nullopt;
    auto found = headerFields->find(fieldName);
    if(found != headerFields->end()) {
        returnValue = found->second;
    }
    return returnValue;
}

void HeaderFieldOwner::addHeaderField(std::string fieldName, std::string fieldValue) {
    LOGGER.trace("HeaderFieldOwner - adding header field: " + fieldName + ": " + fieldValue);
    headerFields->insert_or_assign(fieldName, fieldValue);
}

std::shared_ptr<std::map<std::string, std::string>> HeaderFieldOwner::getAllHeaderFields() {
    return headerFields;
}


}