#pragma once

#include "applicationcontext.h"

namespace level2 {
    class Application {
        public:
            Application() = default;
            static std::shared_ptr<Application> getInstance(const std::string& configFilePath);

    };
}