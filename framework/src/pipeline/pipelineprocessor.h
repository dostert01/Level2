#ifndef SECOND_TAKE_PROCESS
#define SECOND_TAKE_PROCESS

#include <memory>
#include <optional>

#include "pipeline.h"

namespace second_take {

    class Process {
        public:
            Process() = default;
            ~Process();
            static std::unique_ptr<Process> getInstance();
            static std::optional<std::unique_ptr<Process>> getInstance(const std::string configFilePath);
            uint getCountOfPipelines();
        private:
            void loadProcessConfig(const std::string& configFilePath);
    };
}

#endif //SECOND_TAKE_PROCESS
