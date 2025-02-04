#pragma once

#include <memory>
#include <map>
#include <optional>
#include <unistd.h>
#include <vector>

namespace level2 {

class HeaderFieldOwner {
    public:
        HeaderFieldOwner();
        std::optional<std::string> getHeaderFieldValue(std::string fieldName);
        void addHeaderField(std::string fieldName, std::string fieldValue);
        std::shared_ptr<std::map<std::string, std::string>> getAllHeaderFields();
    private:
        std::shared_ptr<std::map<std::string, std::string>> headerFields;
};

} //namespace level2
